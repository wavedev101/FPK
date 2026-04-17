#include "fpk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
    char magic[4];
    uint32_t count;
    char meta[FPK_MAX_META];
    FPKEntry entries[FPK_MAX_FILES];
} FPKHeader;

static int write_all(FILE *f, const void *buf, size_t sz) {
    return fwrite(buf, 1, sz, f) == sz;
}

static int read_all(FILE *f, void *buf, size_t sz) {
    return fread(buf, 1, sz, f) == sz;
}

int fpk_pack(const char *archive, const char **files, int file_count, const char *meta) {
    if (file_count <= 0 || file_count > FPK_MAX_FILES) return -1;

    FILE *out = fopen(archive, "wb");
    if (!out) return -1;

    FPKHeader hdr = {0};
    memcpy(hdr.magic, FPK_MAGIC, 4);
    hdr.count = file_count;
    if (meta) strncpy(hdr.meta, meta, FPK_MAX_META-1);

    size_t header_size = sizeof(FPKHeader);
    fseek(out, header_size, SEEK_SET);

    for (int i = 0; i < file_count; i++) {
        FILE *in = fopen(files[i], "rb");
        if (!in) { fclose(out); return -1; }

        fseek(in, 0, SEEK_END);
        long sz = ftell(in);
        rewind(in);

        unsigned char *buf = malloc(sz);
        fread(buf, 1, sz, in);
        fclose(in);

        hdr.entries[i].offset = ftell(out);
        hdr.entries[i].size = sz;
        strncpy(hdr.entries[i].name, files[i], FPK_MAX_NAME-1);

        write_all(out, buf, sz);
        free(buf);
    }

    fseek(out, 0, SEEK_SET);
    write_all(out, &hdr, sizeof(hdr));
    fclose(out);
    return 0;
}

int fpk_list(const char *archive) {
    FILE *f = fopen(archive, "rb");
    if (!f) return -1;

    FPKHeader hdr;
    if (!read_all(f, &hdr, sizeof(hdr))) { fclose(f); return -1; }
    if (memcmp(hdr.magic, FPK_MAGIC, 4) != 0) { fclose(f); return -1; }

    printf("Archive meta: %s\n", hdr.meta);
    printf("Files: %u\n", hdr.count);

    for (uint32_t i = 0; i < hdr.count; i++) {
        FPKEntry *e = &hdr.entries[i];
        printf("%s (%u bytes)\n", e->name, e->size);
    }

    fclose(f);
    return 0;
}

int fpk_extract(const char *archive) {
    FILE *f = fopen(archive, "rb");
    if (!f) return -1;

    FPKHeader hdr;
    if (!read_all(f, &hdr, sizeof(hdr))) { fclose(f); return -1; }
    if (memcmp(hdr.magic, FPK_MAGIC, 4) != 0) { fclose(f); return -1; }

    for (uint32_t i = 0; i < hdr.count; i++) {
        FPKEntry *e = &hdr.entries[i];
        fseek(f, e->offset, SEEK_SET);

        unsigned char *data = malloc(e->size);
        fread(data, 1, e->size, f);

        FILE *out = fopen(e->name, "wb");
        if (!out) { free(data); continue; }
        fwrite(data, 1, e->size, out);

        free(data);
        fclose(out);
    }

    fclose(f);
    return 0;
}
