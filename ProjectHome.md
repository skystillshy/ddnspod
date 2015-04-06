类似于花生壳的动态域名计划。

&lt;BR&gt;


使用DNSpod提供的API，实时更新本地的IP(v4/v6)到DNSpod上托管的域名(A记录/AAAA记录)。

&lt;BR&gt;



如有任何bug请到<a href='http://code.google.com/p/ddnspod/issues/list'>Issues</a>页面反馈

&lt;BR&gt;



<a href='http://groups.google.com/group/ddnspod'>DDNSpod讨论区，欢迎反馈意见</a>

&lt;BR&gt;



下一步可能改进的方向：https通信；更加准确的IPv6探测机制；对任何记录进行增、删、改操作而不只是局限于A/AAAA记录

&lt;BR&gt;



0.4.3版本

&lt;BR&gt;


-修改API版本号为1.6

&lt;BR&gt;


（官方升级了api，Domain.List和Record.List两个api新增加两个参数：获取域名或记录的每页数量以及偏移量，但从实际情况来看，只要记录数量不是特别多，忽略这两个参数，不会对正确获取造成影响。0.4.3版代码没有什么修改，倘若在使用过程中出错，请及时联系，特别是单个域名的记录数量在20以上的用户）

0.4.2版本

&lt;BR&gt;


-改进获取ADSL拨号上网（PPPOE）IP的方法

0.4.1版本

&lt;BR&gt;


-增加判断ADSL拨号上网（PPPOE）并获取其外网IP的功能（不包括路由器拨号）

&lt;BR&gt;


-稍微改进获取记录的ID的准确度（多个记录重名时）

0.4版本

&lt;BR&gt;


-更新DNSpod API封装类

&lt;BR&gt;


-增加检查API版本功能

&lt;BR&gt;


注意：由于官方API升级，参数发生改变，0.3版本以下无法再更新域名

&lt;BR&gt;



0.3版本

&lt;BR&gt;


-增加更新IPv6域名功能（如果本地IPv6地址错误，会将AAAA记录更新为::1，因此请小心启用）

&lt;BR&gt;


-实现两种自动更新模式

&lt;BR&gt;


-「只自动更新一次域名并自动退出」：程序在100秒后自动更新并且自动退出

&lt;BR&gt;


-「自动更新域名」：每30分钟自动更新一次

&lt;BR&gt;


-自动更新模式不检查帐号、域名、记录是否正确，使用前请准确配置好(在配置对话框设置完毕之后，进行一次“立即更新”，config.xml会自动保存Domain ID和Record ID）

&lt;BR&gt;


-开机自动启动：程序会最小化至托盘

&lt;BR&gt;


-修正几个bug

&lt;BR&gt;


-建议组合：开机自动启动+只自动更新一次域名并自动退出

&lt;BR&gt;



0.2版本

&lt;BR&gt;


-增加主界面登录，保存当前用户所有域名所有记录功能

&lt;BR&gt;


-完善DNSpod API的封装

&lt;BR&gt;



0.1版本

&lt;BR&gt;


-实现了v4 IP手动更新至域名

&lt;BR&gt;

