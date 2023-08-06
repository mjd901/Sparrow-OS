#include"bitmap.h"
#include"stdint.h"
#include"string.h"
#include"print.h"
#include"interrupt.h"
#include"debug.h"

/*初始化设置为0,*/
void bitmap_init(struct bitmap* btmp){
    memset(btmp->bits,0,btmp->btmp_bytes_len);
}
/*判断bit_idx是否为1,是返回true,否在返回flase*/
bool bitmap_scan_test(struct bitmap* btmp,uint32_t bit_idx){
    //获取位图数组的下标索引
    uint32_t byte_idx = bit_idx/8;
    uint32_t bit_odd = bit_idx%8;
    return (btmp->bits[byte_idx]&(BITMAP_MASK)<<bit_odd);
}

/*在位图中申请cnt个位，如果成功返回起始下标，失败返回-1 */

int bitmap_scan(struct bitmap* btmp,uint32_t cnt){
    uint32_t idx_byte = 0; //记录空闲所在的字节
    while((0xff==btmp->bits[idx_byte])&&(idx_byte<btmp->btmp_bytes_len)){
        idx_byte++;
    }
    ASSERT(idx_byte<btmp->btmp_bytes_len);
    if (idx_byte == btmp->btmp_bytes_len)
    {
        return -1;
    }
    //若在位图数组范围内的某字节内找到了空闲位
    //在该字节内逐位比对，返 回空闲位的索引 。
    uint8_t idx_bit=0;
    //和 btmp→bis [ idx_byte ］这个字节逐位对比
    while((uint8_t)(BITMAP_MASK<<idx_bit)&btmp->bits[idx_byte]){
        idx_bit++;
    }
    int bit_idx_start=idx_byte*8+idx_bit; //／／空闲位在位图内的下标

    if(cnt==1){
        return bit_idx_start;
    }
    uint32_t bit_left = (btmp->btmp_bytes_len*8 -bit_idx_start); 
    uint32_t next_bit = bit_idx_start +1;
    uint32_t count=1; //记录找到空闲位数的个数

    bit_idx_start = -1 ;//先将其置为一1 ，若找不到连续的位就直接返回
    while(bit_left-- > 0){
        if(!(bitmap_scan_test(btmp,next_bit))){
            count++;
        }else{
            count=0;
        }
        if(count==cnt){
            bit_idx_start = next_bit-cnt+1;
            break;
        }
        next_bit++;
    }
    return bit_idx_start;
}

/*将位图的bit_idx设置为value*/
void bitmap_set(struct bitmap* btmp,uint32_t bit_idx,uint8_t value){
    ASSERT((value == 0)||(value==1));
    uint32_t byte_idx =bit_idx/8;
    uint8_t bits_odd =bit_idx%8;
    if(value){
        btmp->bits[byte_idx] |=(BITMAP_MASK<<bits_odd); 
    }else{
        btmp->bits[byte_idx] &=~(BITMAP_MASK<<bits_odd); 
    }
}

