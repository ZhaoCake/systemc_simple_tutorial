# Makefile for this tutorial all project
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

# SystemC项目总Makefile
SUBDIRS = mux_4to1 alu_4bit register_ram fifo_design
BUILD_DIR = build

.PHONY: all clean $(SUBDIRS) prepare run $(patsubst %,run-%,$(SUBDIRS))

# 确保子目录目标依赖于prepare，确保构建目录已创建
all: prepare
	@for dir in $(SUBDIRS); do \
		echo "编译 $$dir"; \
		mkdir -p $(BUILD_DIR)/$$dir; \
		$(MAKE) -C $$dir BUILD_DIR=$(realpath $(BUILD_DIR))/$$dir || exit 1; \
	done

# 创建顶级构建目录
prepare:
	@mkdir -p $(BUILD_DIR)

# 运行所有测试
run: all
	@echo "====== 运行所有测试 ======"
	@for dir in $(SUBDIRS); do \
		echo "\n====== 运行 $$dir 测试 ======"; \
		$(MAKE) -C $$dir run BUILD_DIR=$(realpath $(BUILD_DIR))/$$dir || exit 1; \
	done
	@echo "\n====== 所有测试运行完成 ======"

# 单独运行特定模块的测试
$(patsubst %,run-%,$(SUBDIRS)): run-%: prepare
	@echo "====== 编译和运行 $* 测试 ======"
	@mkdir -p $(BUILD_DIR)/$*
	@$(MAKE) -C $* BUILD_DIR=$(realpath $(BUILD_DIR))/$*
	@$(MAKE) -C $* run BUILD_DIR=$(realpath $(BUILD_DIR))/$*

# 清理编译产物
clean:
	rm -rf $(BUILD_DIR)
	@echo "清理完成"
