# Makefile for FIFO design
# Copyright (C) 2025  ZhaoCake

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

# FIFO设计 Makefile

# 编译器和标志
CXX = g++
CXXFLAGS = -std=c++17 -Wall -I/usr/include
LDFLAGS = -L/usr/lib -lsystemc -pthread -Wl,-rpath,/usr/lib

# 构建目录（由上级Makefile传入）
BUILD_DIR ?= $(CURDIR)/../build/fifo_design

# 目标可执行文件
TARGET = $(BUILD_DIR)/fifo_tb

# 源文件和目标文件
SRCS = fifo_tb.cpp
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
