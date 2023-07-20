<h1 align="center" style="border-bottom: none;">📦⚡️casdoor cpp qt example</h1>
<h3 align="center">An example of casdoor-cpp-sdk</h3>

## Architecture

Example contains 2 parts:

| Name     | SDK             | Language | Source code                                       |
| -------- | --------------- | -------- | ------------------------------------------------- |
| Frontend | Qt SDK          | Qt       | https://github.com/casdoor/casdoor-cpp-qt-example |
| Backend  | casdoor-cpp-sdk | c++      | https://github.com/casdoor/casdoor-cpp-sdk        |

## Installation

Example uses Casdoor to manage members. So you need to create an organization and an application for the example in a Casdoor instance.

### Get the code

```shell
git clone https://github.com/casdoor/casdoor
git clone https://github.com/casdoor/casdoor-cpp-qt-example
```

### windows：

win11 + MSVC2019 + Qt5.15

The `webenginewidgets` can only work on MSVC207/2019, no support for  MINGW support.

1. Install MSVC 2019:

   [reference](https://blog.csdn.net/zjgo007/article/details/114296400)

    [Build Tools for Visual Studio 2019](https://my.visualstudio.com/Downloads?q=visual studio 2019&wt.mc_id=o~msft~vscom~older-downloads)

   [Visual C++ Redistributable for Visual Studio 2019](https://my.visualstudio.com/Downloads?q=visual studio 2019&wt.mc_id=o~msft~vscom~older-downloads)

   

2. Install Qt: using Qt Online installer:

   `[Get and Install Qt with Qt Online Installer](https://doc.qt.io/qt-6/qt-online-installation.html)

   use mirror to download Qt: `.\qt-unified-windows-x86-online.exe --mirror https://mirrors.ustc.edu.cn/qtproject`

3. Add include path in `.pro` file. 

   ```qt
   // in ./casdoor-cpp-qt-example.pro
   INCLUDEPATH += $$quote(D:/Program Files/OpenSSL-Win64/include) // installation path of OpenSSL
   ```


### Linux

Ubuntu22.04 + gcc + Qt5.15

1. Install Qt

   `[Get and Install Qt with Qt Online Installer](https://doc.qt.io/qt-6/qt-online-installation.html)

   use mirror to download Qt: `.\qt-unified-windows-x86-online.exe --mirror https://mirrors.ustc.edu.cn/qtproject`

2. Install Openssl

   Install from Source

   ```
   git clone git://git.openssl.org/openssl.git
   cd openssl
   ./Configure --prefix=/usr/local/openssl
   make && make install
   ```

3. Add include path in `.pro` file. 

   ```
   // in ./casdoor-cpp-qt-example.pro
   INCLUDEPATH += "/user/local/openssl/include/openssl" // installation path of OpenSSL
   ```

   

   

### Configure Application in Casdoor

1. Setup your Casdoor Applicaiton

   Set redirect URL for your Applicaiotn :  eg. `http://localhost:11000/callback`

   Remember the `Client ID` and `Client secert` in the Application page

   <img src="https://article.biliimg.com/bfs/article/799f0216c760ffccec3e1fa2da2aea23a7880428.png" style="zoom:67%;" />

   Copy the public `Certificate` in the Cert page.

   <img src="https://article.biliimg.com/bfs/article/1703e611fa726da947a99f6bdd3449e76953c695.png" style="zoom:66%;" />

### Configure  Casdoor SDK

1. Configure SDK

   ```cpp
   // in ./mainwindow.h
   std::string endpoint = "http://localhost:7001"; //front-end
   std::string client_id = "3386b5767bbedeecd9d9";
   std::string client_secret = "2a576a2fc4293d5da8f10e1e42399fc973a50ba6";
   std::string certificate = R"(-----BEGIN CERTIFICATE-----
   -----END CERTIFICATE-----)";
   std::string org_name = "casbin-forum";
   std::string app_name = "app-qt";
   std::string redirect_url = "http://localhost:11000/callback";
   std::string response_type = "code";
   std::string scope = "read";
   ```


   ```cpp
   // in ./mainwindow.cpp
   
   // MainWindow::initTcpServer
   m_tcpserver->listen(QHostAddress::LocalHost, 8080); // port where tcp server listen
   
   // MainWindow::on_auth_code_received
   if(url.toString().startsWith("http://localhost:11000/callback"))  // redirect_url
   ```


   ```cpp
   // in ./mainwindow.cpp
   CasdoorConfig* casdoor = new CasdoorConfig(
       "http://localhost:7001", // Casdoor Frontend Url
       "3efd29ff3e0b14ba1dd7", // client id
       "34cb65d634b06a49f14c6bc49884ce1df55ce518", // client secret
       cert, // certificate
       "built-in" // organization
   );
   ```


5. Run Qt Application

   * login: 

   <img src="https://article.biliimg.com/bfs/article/f3102273db5e9c98303350c3a6a2be20b595d3e1.png" style="zoom:67%;" />

   * result: 

   <img src="https://article.biliimg.com/bfs/article/b87d28f20d0224860da7155b2936a2eaad5200a6.png" style="zoom: 67%;" />
