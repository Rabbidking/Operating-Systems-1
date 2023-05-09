.intel_syntax noprefix

.text
pop esi /*get ptr to boot info*/
lea esp, mystack
lea eax, _bssEnd
push eax
lea eax, _bssStart
push eax
call _clearBss
add esp,8
push esi /*pass as parameter to kmain*/
call _kmain

.data
.skip 16384
mystack: