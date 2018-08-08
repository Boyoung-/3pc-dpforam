# 3pc-dpforam
C++ Implementation of 3PC-DPF-ORAM

Setup:
$ ./setup.sh

Usage:
$ cd build
$ ./proto_test [OPTION...]
      --par arg    Party:[eddie|debbie|charlie]
      --proto arg  Protocol:[dpforam|ssot|inslbl] (default: dpforam)
      --eip arg    Eddie's ip (default: 127.0.0.1)
      --dip arg    Debbie's ip (default: 127.0.0.1)
      --tau arg    Tau (default: 3)
      --logn arg   LogN (default: 12)
      --db arg     DBytes (default: 4)
      --thr arg    Threads (default: 1)
      --iter arg   Iterations (default: 100)
      
Sample Run (localhost):
$ ./proto_test --par eddie --logn 12 &
$ ./proto_test --par debbie --logn 12 &
$ ./proto_test --par charlie --logn 12 &

Sample Run (remote servers):
(1.2.3.4)$ ./proto_test --par eddie --logn 12
(5.6.7.8)$ ./proto_test --par debbie --logn 12 --eip 1.2.3.4
(9.0.1.2)$ ./proto_test --par charlie --logn 12 --eip 1.2.3.4 --dip 5.6.7.8

Ports Used: 8000-8002