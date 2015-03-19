We will assume that YB.ORM resides at `c:\yborm`.

Select “All configurations”

![http://yb-orm.googlecode.com/git/doc/pics/msvc-settings_html_m37ea733d.png](http://yb-orm.googlecode.com/git/doc/pics/msvc-settings_html_m37ea733d.png)


At “Configuration Properties/General/Character Set” select ”Not Set”

![http://yb-orm.googlecode.com/git/doc/pics/msvc-settings_html_33910ae5.png](http://yb-orm.googlecode.com/git/doc/pics/msvc-settings_html_33910ae5.png)


At “Configuration Properties/Debugging/Environment” add `“c:\yborm\bin”` to your project's `PATH`

![http://yb-orm.googlecode.com/git/doc/pics/msvc-settings_html_53d3cb51.png](http://yb-orm.googlecode.com/git/doc/pics/msvc-settings_html_53d3cb51.png)


At “Configuration Properties/VC++ Directories/Include Directories” add `“c:\yborm\include\yb”`, `“c:\yborm\include”` and `$(ProjectDir)`

![http://yb-orm.googlecode.com/git/doc/pics/msvc-settings_html_701e5726.png](http://yb-orm.googlecode.com/git/doc/pics/msvc-settings_html_701e5726.png)


At “Configuration Properties/VC++ Directories/Library Directories” add `“c:\yborm\lib”`

![http://yb-orm.googlecode.com/git/doc/pics/msvc-settings_html_32f49c2b.png](http://yb-orm.googlecode.com/git/doc/pics/msvc-settings_html_32f49c2b.png)


At “Configuration Properties/C/C++/Precompiled Headers choose not to use this feature

![http://yb-orm.googlecode.com/git/doc/pics/msvc-settings_html_2311dde8.png](http://yb-orm.googlecode.com/git/doc/pics/msvc-settings_html_2311dde8.png)


At “Configuration Properties/Linker/Input/Additional Dependencies” add YB.ORM libraries and friends:
  * `ybutil.lib`
  * `yborm.lib`
  * `libxml2.lib`
  * `libboost_date_time-vc100-mt-gd-1_46_1.lib`
  * `libboost_thread-vc100-mt-gd-1_46_1.lib`

![http://yb-orm.googlecode.com/git/doc/pics/msvc-settings_html_64d1c5b6.png](http://yb-orm.googlecode.com/git/doc/pics/msvc-settings_html_64d1c5b6.png)