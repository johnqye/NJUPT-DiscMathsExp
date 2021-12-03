CFLAGS = -O2 -Wall -Werror
VISDIR = vis
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
BIN = Exp4.elf
# 默认参数（图）
ARGS = abcdefgh aabbcddeefg bdceefgghgh 128

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@
# 生成并可视化图的所有欧拉回路，保存为svg矢量图
vis: $(BIN) cleanVis
	mkdir -p $(VISDIR)
	cd $(VISDIR); ../$(BIN) $(ARGS) | dot -Tsvg -O
	mv $(VISDIR)/noname.gv.svg $(VISDIR)/Original.svg
	cd $(VISDIR); rename 's/noname.gv.//' * >/dev/null 2>&1
# 清理所有东西
clean: cleanBin cleanVis 
# 清理生成的可执行文件
cleanBin:
	$(RM) $(BIN) $(OBJ)
# 清理欧拉回路矢量图
cleanVis:
	$(RM) -r $(VISDIR)

all: $(BIN)
