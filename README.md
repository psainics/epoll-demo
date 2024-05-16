## Epoll Demo

- Epoll is the famous I/O event notification used by many apps to make async I/O possible.
- Epoll is a syscall that let's us monitor multiple file descriptors to see if I/O is possible on any of them.
- This is a simple chatroom server that uses epoll to monitor multiple clients.
- Only for educational purposes. Do not use in production.
- Read about epoll [here](https://man7.org/linux/man-pages/man7/epoll.7.html)

### Demo
- Image below shows 3 cliens in a chatroom !

  ![image](https://github.com/psainics/epoll-demo/assets/122770897/2efcf54b-386f-45fa-b272-fe933a206d3c)
