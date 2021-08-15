#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>

long get_symtab_command(FILE *fd, struct symtab_command *sc);

uint32_t replace_symbol(FILE *fd, struct symtab_command sc, const char *old_symbol, const char *new_symbol);

int main(int argc, const char * argv[]) {
    
    const char *path = argv[1];
    const char *old_symbol = argv[2];
    const char *new_symbol = argv[3];
    
    FILE *fd = fopen(path, "rb+");
    struct symtab_command *sc = malloc(sizeof(struct symtab_command));
    long sc_position = get_symtab_command(fd, sc); // 找到 .symtab
    if (sc_position <= 0) return 1;
    
    // 替换符号表为新字符串，并返回新的字符串大小
    uint32_t new_strsize = replace_symbol(fd, *sc, old_symbol, new_symbol);
    if (new_strsize > 0) {
        sc->strsize = new_strsize;
        fseek(fd, sc_position, SEEK_SET);
        fwrite(sc, sizeof(struct symtab_command), 1, fd);
    }
    fclose(fd);
    return 0;
}

long get_symtab_command(FILE *fd, struct symtab_command *sc) {
    struct mach_header_64 mh;
    
    fread(&mh, sizeof(struct mach_header_64), 1, fd);
    size_t load_command_size = sizeof(struct load_command);
    
    for (int i = 0; i < mh.ncmds; ++i) {
        struct load_command lc;
        fread(&lc, load_command_size, 1, fd);
        switch (lc.cmd) {
            case LC_SYMTAB:
                fseek(fd, -load_command_size, SEEK_CUR);
                long position = ftell(fd);
                fread(sc, sizeof(struct symtab_command), 1, fd);
                return position;
            default:
                fseek(fd, lc.cmdsize - load_command_size, SEEK_CUR);
                break;
        }
    }
    return 0;
}

uint32_t replace_symbol(FILE *fd, struct symtab_command sc, const char *old_symbol, const char *new_symbol) {
    // 读取全部的 String Table
    char strings[sc.strsize];
    fseek(fd, sc.stroff, SEEK_SET);
    fread(strings, sizeof(char), sc.strsize, fd);
    // 获取当前对齐信息
    uint32_t old_align_count = 0;
    while (!strings[sc.strsize - 1 - old_align_count]) {
        old_align_count++;
    }
    
    size_t nlist_64_size = sizeof(struct nlist_64);
    // 遍历 Symbol Table
    fseek(fd, sc.symoff, SEEK_SET);
    for (int i = 0; i < sc.nsyms; ++i) {
        struct nlist_64 nl;
        fread(&nl, nlist_64_size, 1, fd);

        const char *string = strings + nl.n_un.n_strx;
        
        if (strcmp(string, old_symbol) != 0) continue;

        // 匹配到字符，String Table 末尾插入新字符串，同时修改 String Table Index 到新位置
        uint32_t new_n_strx = sc.strsize - (old_align_count - 1); // 新的 index
        uint32_t string_offset = sc.stroff + new_n_strx; // 在二进制的写入位置
        
        size_t new_symbol_len = strlen(new_symbol);

        // 修改 String Table Index
        nl.n_un.n_strx = new_n_strx;
        fseek(fd, -nlist_64_size, SEEK_CUR);
        fwrite(&nl, nlist_64_size, 1, fd);
        // 插入新字符串
        fseek(fd, string_offset, SEEK_SET);
        fwrite(new_symbol, sizeof(char), new_symbol_len, fd);
        
        // 重新写入对齐信息
        uint32_t align_count = 8 - (new_n_strx + (int)(new_symbol_len * sizeof(char))) % 8;
        char end[7] = {};
        fwrite(end, sizeof(char), align_count, fd);
        
        uint32_t new_strsize = sc.strsize - (old_align_count - 1) + (uint32_t)new_symbol_len + align_count;

        return new_strsize;
    }
    
    return 0;
}
