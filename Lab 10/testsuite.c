#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#include "kprintf.h"

extern int file_open(const char* fname, int flags);
extern int file_read(int fd, void* buf, int num);
extern int file_seek(int fd, int offset, int whence);
extern int file_close(int fd);


unsigned adler32(int fd){
    unsigned char buf[23];
    unsigned short v1=1;
    unsigned short v2=0;
    while(1){
        int rv = file_read(fd,buf,sizeof(buf));
        if( rv < 0 ){
            kprintf("Error\n");
            return 0;
        }
        if( rv > sizeof(buf) ){
            kprintf("Error\n");
            return 0;
        }
        
        if( rv == 0 ){
            return (v2 << 16) | v1;
            break;
        }
        int i;
        for(i=0;i<rv;++i){
            v1 = (v1 + buf[i]) % 65521;
            v2 = (v2 + v1) % 65521;
        }
    }
}

void sweet(){
    int fds[7];
    
    const char* fnames[] = {
        "article1.txt",
        "article2.txt",
        "article3.txt",
        "article6.txt",
        "article5.txt",
        "article4.txt",
        "const.txt",
        0
    };
    unsigned adlers[] = {
        0xea636d6c, 0x624b3183, 0x2aa1e679, 
        0x7ef154ca, 0x642a321d, 0x6cada551,
        0x17685d9f, 0
    };
        
    int i;
    for(i=0;fnames[i];++i){
        fds[i] = file_open(fnames[i],0);
        if( fds[i] < 0 ){
            kprintf("Could not open %s\n",fnames[i]);
            return ;
        }
        unsigned a = adler32(fds[i]);
        if( a != adlers[i] ){
            kprintf("%s does not match: %x %x\n", fnames[i],
                a, adlers[i] );
            return;
        }
    }
    
    for(i=0;fnames[i];++i){
        if( 0 != file_close(fds[i]) ){
            kprintf("Close error\n");
            return;
        }
    }
    
    kprintf("\n\n\n !!! OK !!! \n\n\n");
    return;
}
