# TicketSystem:Automata 软件开发手册

版本: `V 0.1`		最新更新: `2021-6-6`		编写者: `PaperL`

## 一、项目说明

### 1.1 开发目的

- 数据结构课程大作业 [TicketSystem-2021](https://github.com/IvyCharon/TicketSystem-2021)

### 1.2 开发人员

- 上海交通大学, 2020级, ACM班, [RainyMemory](https://github.com/Rainy-Memory) & [PaperL](https://github.com/PaperL)

### 1.3 开发目标

- 实现具有部分功能的火车票订票系统
  - 开发后端 `C++` 程序
    - 开发车票系统逻辑部分
    - 实现文件存储数据结构 `B+树`
  - 开发前端界面及配套文档
    - 提供用户友好的网页界面
    - 提供网站部署文档

##  二、开发文档

### 2.1 后端开发文档

- 详见 [DevelopmentDocument.md](https://github.com/Rainy-Memory/TicketSystem-Automata/blob/master/General/DevelopmentDocument.md)

### 2.2 前端开发文档

- 2.2.1 服务器端文件结构
  - BackEnd
    - 后端可执行文件
  - FrontEnd `10 Files`
    - `index.html`, `login.html`, `register.html`
      - 非登录用户界面，主页与账号相关功能
    - `dashboard.html`, `dashboard*.html`
      - `*-ticket.html`, `*-order.html`, `*-train.html`, `*-user.html`, `*-modifyProfile.html`
      - 登录用户界面，订票相关功能
    - `console.html`
      - 管理员界面，后端控制台功能
    - js
      - `script.js`
        - 含有操作 `Cookie` ，加载 `Dashboard` 界面用户名，退出登录全局函数
      - `MDB` 库文件
    - php `12 Files`
      - User Profile: `register.php`, `login.php`, `logout.php`, `search.php`, `modify.php`
      - Ticket: `query.php`, `queryTrans.php`
      - Order: `buy.php`, `order.php`, `refund.php`
      - Train: `train.php`
      - `console.php`
      - 将前端表单内容以字符串形式拼接为指令传至后端。获得后端运行结果直接返回前端。
    - img
      - `gear.ico`
      - 网站图标
    - css
      - `MDB` 库文件
  - Software
    - nginx-1.17.7
    - php-7.4.19-Win32-vc15-x64

- 2.2.2 前后端通信方式
  - `XMLHttpRequest` 异步通信
- 2.2.3 网页更新信息方式
  - 网页内嵌 `JavaScript` 动态加载
- 2.2.4 视觉效果设计
  - 2.2.4.1 设计目标
    - 易用、美观
  - 2.2.4.2 设计规范
    - 设计过程遵循 [`Material Design`](https://material.io/design/introduction) 规范
    - `html` 元素使用 `MDB` 控件
    - 颜色采用 `Material Design` 色彩系统

## 三、部署手册

### 3.1 系统环境

- 3.1.1 最低配置需求
  - 操作系统: `Windows 7/8.1/10` `64bit`
  - 处理器: `Intel Core i3 2100` 或 `AMD A8-6500`
  - 内存: `4 GB`
  - 网络: 宽带互联网连接
  - 存储空间: 需要 `50 GB` 可用空间
- 3.1.2 软件需求
  - `nginx` 版本 `1.17.7`
  - `php` 版本 `7.4.19-Win32-vc15-x64`

### 3.2 部署方式

- 3.2.1 准备工作
  - 安装 `nginx` 及 `php` 至系统
- 3.2.2 配置 `nginx` 及 `php` 配置文件
  - nginx
    - 修改软件安装目录下 `nginx-1.17.7\conf\nginx.conf` 文件
      - `http -> server -> location/ -> root` 为 `FrontEnd` 文件夹目录(包含 `FrontEnd` 文件夹名)
      - `http -> server -> location ~ \.php$` 内容如下，共 `5` 句
        - `root` 为 `FrontEnd` 文件夹目录(包含 `FrontEnd` 文件夹名)
        - `fastcgi_pass` 为 `127.0.0.1:9000`
        - `fastcgi_index` 为 `\php\index.php`
        - `fastcgi_param  SCRIPT_FILENAME $document_root$fastcgi_script_name;`
        - `include        fastcgi_params;`
  - php
    - 修改软件安装目录下 `php-7.4.19-Win32-vc15-x64\php.ini-development` 文件
      - 文件名: 修改为 `php.ini`
      - `line 761`: 取消该行注释 (删除行首分号)，并修改 `extension_dir` 为软件安装目录下 `php-7.4.19-Win32-vc15-x64\ext` 文件夹的绝对路径(包含文件夹名)
      - `line 778`: 取消 `cgi.force_redirect = 1` 注释
      - `line 798`: 取消 `cgi.fix_pathinfo=1` 注释
      - `line 810`: 取消 `fastcgi.impersonate = 1` 注释
      - `line 944`: 取消 `extension=sockets` 注释
- 3.2.3 启动系统
  - 后端
    - 在 `wsl` 中打开后端可执行文件所在目录，执行指令 `./code`
  - nginx
    - 双击安装目录下 `nginx.exe` 
  - php
    - 在 `cmd` 中打开安装目录，执行指令 `php-cgi.exe -b 127.0.0.1:9000 -c php.ini`
- 3.2.4 关闭系统
  - 后端及php
    - 关闭终端即可
  - nginx
    - 使用任务管理器强制结束所有名为 `nginx.exe` 的进程
    - 或者在 `cmd` 中使用指令 `taskkill /im nginx.exe /f` 强制结束进程

### 3.3 访问方式
- 3.3.1 后端
  - 参考终端输出提醒信息
- 3.3.2 前端
  - 在浏览器地址栏输入 `localhost` 访问
  - 如需访问指定网页，在浏览器地址栏输入例如 `localhost/index.html` 地址

## 四、参考资料

- `html` 设计参考自: [Material Design for Bootstrap 5](https://mdbootstrap.com/docs/standard/)
- 图片素材参考自: [NieR:Automata](https://nierautomata.square-enix-games.com/)

