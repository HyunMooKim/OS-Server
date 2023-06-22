## Implementation of simple webserver.( a subset of HTTP/1.1 )
1. only GET Method is allowed.<br>
2. I/O multiplexing for the multiple clients.<br>
3. environment: linux, C <br><br>
## how to run
gcc webserver.c -o webserver
./webserver 8080 resources
<br>
or by <br> 
sh run.sh
<br>

screen for the proper request <br>
![image](https://github.com/HyunMooKim/OS-Server/assets/100335266/9b949eae-124f-43c3-b37d-af39a24a3bc1)


screen for the improper request <br> 
![image](https://github.com/HyunMooKim/OS-Server/assets/100335266/ede7ffce-05fc-418e-a8eb-c08ee73f604c)

what happens if you ask POST Method <br>
![image](https://github.com/HyunMooKim/OS-Server/assets/100335266/cb1a5058-944e-45f8-a265-fa85bd3e9550)

what happens if you ask for requests other than POST, GET <br>
![image](https://github.com/HyunMooKim/OS-Server/assets/100335266/a9f083a1-91ff-45d8-ae6a-6102e2713e26)





