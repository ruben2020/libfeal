Write-Host "================================="
Write-Host "Running fealactorstimers ..."
Start-Process 'D:\a\libfeal\libfeal\build\examples\actors_and_timers\fealactorstimers.exe' -NoNewWindow -Wait
Write-Host "================================="
Write-Host "Running promise ..."
Start-Process 'D:\a\libfeal\libfeal\build\examples\promise\fealpromise.exe' -NoNewWindow -Wait
Write-Host "================================="
Write-Host "Running tcp IPv4 ..."
Start-Process 'D:\a\libfeal\libfeal\build\examples\tcp_echo_clientserver\fealtcpclient.exe' -NoNewWindow 
Start-Process 'D:\a\libfeal\libfeal\build\examples\tcp_echo_clientserver\fealtcpserver.exe' -NoNewWindow -Wait
Write-Host "================================="
Write-Host "Running tcp IPv6 ..."
Start-Process 'D:\a\libfeal\libfeal\build\examples\tcp_echo_clientserver\fealtcpclient6.exe' -NoNewWindow 
Start-Process 'D:\a\libfeal\libfeal\build\examples\tcp_echo_clientserver\fealtcpserver6.exe' -NoNewWindow -Wait
Write-Host "Running udp IPv4 ..."
Start-Process 'D:\a\libfeal\libfeal\build\examples\udp_echo_clientserver\fealudpclient.exe' -NoNewWindow 
Start-Process 'D:\a\libfeal\libfeal\build\examples\udp_echo_clientserver\fealudpserver.exe' -NoNewWindow -Wait
Write-Host "================================="
Write-Host "Running udp IPv6 ..."
Start-Process 'D:\a\libfeal\libfeal\build\examples\udp_echo_clientserver\fealudpclient6.exe' -NoNewWindow 
Start-Process 'D:\a\libfeal\libfeal\build\examples\udp_echo_clientserver\fealudpserver6.exe' -NoNewWindow -Wait
Write-Host "Running tls IPv4 ..."
Start-Process 'D:\a\libfeal\libfeal\build\examples\tls_echo_clientserver_openssl\fealtlsclient.exe' -WorkingDirectory 'D:\a\libfeal\libfeal\build\examples\tls_echo_clientserver_openssl' -NoNewWindow 
Start-Process 'D:\a\libfeal\libfeal\build\examples\tls_echo_clientserver_openssl\fealtlsserver.exe' -WorkingDirectory 'D:\a\libfeal\libfeal\build\examples\tls_echo_clientserver_openssl' -NoNewWindow  -Wait
Write-Host "================================="
Write-Host "Running tls IPv6 ..."
Start-Process 'D:\a\libfeal\libfeal\build\examples\tls_echo_clientserver_openssl\fealtlsclient6.exe' -WorkingDirectory 'D:\a\libfeal\libfeal\build\examples\tls_echo_clientserver_openssl' -NoNewWindow 
Start-Process 'D:\a\libfeal\libfeal\build\examples\tls_echo_clientserver_openssl\fealtlsserver6.exe' -WorkingDirectory 'D:\a\libfeal\libfeal\build\examples\tls_echo_clientserver_openssl' -NoNewWindow  -Wait
