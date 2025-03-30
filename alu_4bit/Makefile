# 4位带符号补码ALU Makefile

# 编译器和标志
CXX = g++
CXXFLAGS = -std=c++17 -Wall -I/usr/include
LDFLAGS = -L/usr/lib -lsystemc -pthread -Wl,-rpath,/usr/lib

# 构建目录（由上级Makefile传入）
BUILD_DIR ?= $(CURDIR)/../build/alu_4bit

# 目标可执行文件
TARGET = $(BUILD_DIR)/alu_4bit_tb

# 源文件和目标文件
SRCS = alu_4bit_tb.cpp
OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# 默认目标
all: $(TARGET)

# 确保构建目录存在
$(BUILD_DIR):
	mkdir -p $@

# 编译和链接规则
$(TARGET): $(OBJS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "编译完成: $@"
	@echo "运行命令: $@"

# 编译规则
$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 运行目标
.PHONY: run
run: $(TARGET)
	$(TARGET)

# 清理目标
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	@echo "清理完成"
