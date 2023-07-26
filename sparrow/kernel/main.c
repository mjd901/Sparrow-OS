int main(){
   char *video_memory = (char *)0xb8000;  // 将0xb8000地址转换成字符指针
    
    // 在屏幕上显示字符 'A'，位于第一行第一列
    video_memory[0] = 'A';
    video_memory[1] = 0x07;  // 字符属性，例如颜色，这里使用默认属性
    while(1);
    return 0;
}