# SCNUCPC 2020

## DOMjudge 部署

在安装 DOMjudge 之前手动装 MariaDB，不要装 MySQL 8.x（否则无法交题）。

接下来先修改 `/etc/default/grub`：

```
GRUB_CMDLINE_LINUX_DEFAULT="quiet splash cgroup_enable=memory swapaccount=1"
```

`update-grub` 重启。

装 DOMjudge 的 DEB 包：

* https://www.domjudge.org/download
* ~~http://10.191.65.243:9000/scnuoj/domjudge/~~

`domjudge-domserver` 是网页应用，`domjudge-judgehost` 是判题机。

安装过程中设定数据库账户密码。

检查 `/etc/domjudge/restapi.secret`：

https://www.domjudge.org/docs/manual/master/install-judgehost.html#rest-api-credentials

如果是 NGINX，配置文件在 `/etc/domjudge/nginx-conf`，放 `/etc/nginx/sites-enabled/`

这个文件 include 了 `/etc/domjudge/nginx-conf-inner`，要把路径的 domjudge 去掉的话：

```
set $prefix '';
location / {
	root $domjudgeRoot;
	try_files $uri @domjudgeFront;
}
```

检查网页应用能否跑起来，尝试登录，密码 `/etc/domjudge/initial_admin_password.secret`

接下来创建 chroot 环境：`/usr/sbin/dj_make_chroot`，直接用本地源（若仍有）：

``` 
[ -z "$DEBMIRROR" ] && DEBMIRROR="http://localhost:9000/ubuntu/"
```

```
sudo systemctl enable domjudge-judgehost
```

尝试交题。

按照 Config checker 自测，改 `/etc/mysql/mariadb.conf.d/50-server.cnf` 的 `max_connections`。

上传大小限额，php-fpm 相关配置，略。NGINX 有自带设置 `client_max_body_size 0` 不用碰。

## 赛前准备

关于放题，赛前验题要不开验题赛要不按下面，验题赛要关外榜限制参加队伍

https://clics.ecs.baylor.edu/index.php?title=Problem_format

<S>

带 Testlib 的题包示例（试机赛 B 题）：

https://bobby285271.coding.net/p/static/d/public/git/raw/master/B.zip

</S>

不使用 SPJ 不需要定义 `validation: 'custom'`，也不需要 `output_validators`

Testlib 需要用蔡队的魔改版：

https://github.com/cn-xcpc-tools/testlib-for-domjudge

复用已有 SPJ 或改编译参数在 `jury/executables`。编译参数默认就是 gnu++14 基本不用碰。

将管理员与队伍绑定并设置不可见 `jury/users/1/edit`，绑定 DOMjudge 组，添加身份 Team Member，Team Categories 相应的组 visible 反选。

关于导入队伍看 `doc/manual/html/import.html` 还有

https://clics.ecs.baylor.edu/index.php?title=Contest_Control_System_Requirements#teams.tsv

首先创建 team，再创建 account，team 要指定 id，account 的用户名要按照 `team-xxxx` 命名，xxxx 是队伍 id，会自动绑定相应队伍。

假设是多个学院打比赛，用 Team Categories 区分，不要用 Team Affiliations，不然就是一个队伍一个 Affiliation 不会合并的。

`jury/users/generate-passwords` 密码这里批量生成，导入时不需要指定。

多赛点需要打印的话，考虑以下打印命令，在 `jury/config`，`print_command`：

```
f=/var/www/scnuoj/domjudge_print && cp [file] $f && cd $f && mv [file] team[teamid]-T$(date +"%m%d%H%M%S").[language] && rm -rf php*
```

`print_command` 不要用 teamname，如果有中文可能会查询不出来，teamid 不好说，真遇到查不出的情况就发公告吧..

变量说明在文档搜 Print 有，打印考虑到要支持不需要连 ssh 就能删文件，最好有在线查看，于是

https://github.com/prasathmani/tinyfilemanager

几个地方要砍，在线编辑重命名，还有什么权限修改什么的，另外把自带的设置和账户系统也砍了，大概是这样，改一下前端，把几个远程加载的静态资源拉下来差不多。

https://github.com/SCNU-SoCoding/scnucpc-print-queue-lister

隐藏文件默认不会显示，怕误删就都隐藏了，然后 baseauth 开了，账户密码

```
htpasswd -c /etc/nginx/.htpasswd socoding
```

```
server {
        listen 9050;
        listen [::]:9050;
        access_log off;
        root /var/www/scnuoj/domjudge_print;
        server_name files_domjudge;
        index .queue.php;
        charset utf-8,gbk;
        location / {
                auth_basic "Admin Area";
                auth_basic_user_file /etc/nginx/.htpasswd;
        }
        location ~ \.php$ {
                include snippets/fastcgi-php.conf;
                fastcgi_pass unix:/var/run/php/php7.4-fpm.sock;
        }
}
```

最后是视图的一些修改，网页应用在 `/usr/share/domjudge/webapp/`，要魔改基本就是 `templates`，例如要改 OJ 名称就把 `base.html.twig` 和 `menu.html.twig` 里面相关的字眼改了就行。

提交的文件名不能有中文，可改 `/usr/share/domjudge/webapp/templates/team/submit_modal.html.twig` 加个提示。

```
<div class="alert alert-info rounded-0">
  Please make sure your filename only contains English letters and Arabic figures.<br>
</div>
```

魔改完使更改马上生效：

```bash
rm -rf /var/cache/domjudge/prod/*
```

滚榜废了，世界未解之谜之一。

https://0xffff.one/d/821
