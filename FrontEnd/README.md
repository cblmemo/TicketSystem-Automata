## TODO

## Total

- 优化各界面除表格外的窄屏(移动端)横向滚动效果，保证输入框和按钮的最低宽度
- 优化 `dashboard` 右上角用户名加载，使用 Cookie 降低重复加载次数，提高加载速度
- 进一步优化美观性 (可能的悬浮导航栏以及返回顶端按钮以及收缩侧栏及更多细节动画效果)
- 添加实用的 `add_train` 及 `release_train` 界面
- 提高安全性 (尽管后端毫无安全性)

### console

- 添加复制 log 按钮

### dashboard-order & dashboard-ticket

- 不知为何手贱把 `table-responsive` 改为`table-responsive-md` 导致横向滚动条失效。