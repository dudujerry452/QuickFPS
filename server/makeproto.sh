# 创建一个存放生成代码的目录
mkdir -p pb 

# 运行 protoc 命令
protoc --go_out=./pb --go_opt=paths=source_relative ./proto/gamedata.proto