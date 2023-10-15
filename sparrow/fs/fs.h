#ifndef __FS_H 
#define __FS_H
#define MAX_FILES_PER_PART 4096 //每个分区最大的文件数
#define BITS_PER_SECTOR 4096 //每扇区的位数512*8bit
#define SECTOR_SIZE 512 //每个扇区的字节
#define BLOCK_SIZE SECTOR_SIZE //块的字节大小

/*文件类型*/
enum file_types{
    FT_UNKNOWN,//不支持的文件类型
    FT_REGULAR,//普通文件
    FT_DIRECTORY//目录
};
void filesys_init(void);
#endif // !_FS_H 