## 关于世界数据导入/导出

客户端/服务端对每个玩家维护last_ticks, 该字段表示最新输入之后物理运算的帧数; 同时对每个玩家维护一个最新输入序列号latest_sequence. 

客户端对本地玩家维护一个输入队列, 该输入队列除了维护输入序列之外还维护每个输入位于其前输入的多少个tick: before_tick. 

每当收到服务端的世界更新广播, 就将所有小于服务端打入的更新世界的该玩家的输入序列号中本地输入队列的元素清除, 得到“还未被确认的输入”的队列. 用该队列在更新世界的基础上重新演算得到当前世界状态. 

值得一提的是, 当服务端给出的last_ticks大于本地玩家“还未被确认输入”队列的最老元素的before_tick, 说明服务端在没接收到玩家给出的某输入的情况下演算过头了. 我对这种情况的处理时打印一条日志, 并在本地立刻演算该“超时”输入. 也算补救吧. 