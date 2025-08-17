cmake --version
ninja --version
md build
cd build
$libssl = Get-ChildItem -Path 'C:\Program Files\OpenSSL\bin' -Include "libssl-3*.dll" -Recurse | Select-Object -ExpandProperty FullName
$libcrypto = Get-ChildItem -Path 'C:\Program Files\OpenSSL\bin' -Include "libcrypto-3*.dll" -Recurse | Select-Object -ExpandProperty FullName
$filePath1 = Get-ChildItem -Path 'C:\ProgramData\chocolatey\lib\curl\tools' -Include 'curl.h' -Recurse | Select-Object -ExpandProperty FullName
$curllibpath = Get-ChildItem -Path 'C:\ProgramData\chocolatey\lib\curl\tools' -Include 'libcurl-x64.dll' -Recurse | Select-Object -ExpandProperty FullName
$curlheaderpath = $filePath1.replace('\curl\curl.h','')
$cmakecmd = -join('cmake -G Ninja -DGHAWIN=ON -DCURL_EX=ON -DOPENSSL_EX=ON -DCURL_LIBRARY="', $curllibpath, '" -DCURL_INCLUDE_DIR="', $curlheaderpath, '" -DOPENSSL_ROOT_DIR="C:\Program Files\OpenSSL" -DGHA_SSLINCL="C:\Program Files\OpenSSL\include" -DGHA_LIBSSL="', $libssl, '" -DGHA_LIBCRYPTO="', $libcrypto, '" ..')
Invoke-Expression $cmakecmd
ninja
Copy-Item -Path 'D:\a\libfeal\libfeal\examples\tls_echo_clientserver_openssl\server.key' -Destination 'D:\a\libfeal\libfeal\build\examples\tls_echo_clientserver_openssl'
Copy-Item -Path 'D:\a\libfeal\libfeal\examples\tls_echo_clientserver_openssl\server.crt' -Destination 'D:\a\libfeal\libfeal\build\examples\tls_echo_clientserver_openssl'
Copy-Item -Path "C:\Program Files\OpenSSL\bin\*.dll" -Destination 'D:\a\libfeal\libfeal\build\examples\tls_echo_clientserver_openssl'
Copy-Item -Path $curllibpath -Destination 'D:\a\libfeal\libfeal\build\examples\promise_curl'
Copy-Item -Path $curllibpath -Destination 'D:\a\libfeal\libfeal\build\examples\descmon_curl'
