#pragma once

#include "error/error.h"
#include "instruction/instruction.h"
#include "tokenizer/token.h"

#include<iostream>
#include <vector>
#include <optional>
#include <utility>
#include <map>
#include <cstdint>
#include<cstring>
#include <cstddef> // for std::size_t

namespace miniplc0 {

	class Analyser final {
	private:
		using uint64_t = std::uint64_t;
		using int64_t = std::int64_t;
		using uint32_t = std::uint32_t;
		using int32_t = std::int32_t;
	public:
		Analyser(std::vector<Token> v)
			: _tokens(std::move(v)), _offset(0), _instructions({}), _current_pos(0, 0), _void_func({}), _jump_stack({}),
			globalTable({}), localTable({}), _functions({}), _nextTokenIndex(0), _functionIndex(0), isGlobal(true) {}	//初始化
		Analyser(Analyser&&) = delete;
		Analyser(const Analyser&) = delete;
		Analyser& operator=(Analyser) = delete;

		// 唯一接口
		std::pair<std::vector<Instruction>, std::optional<CompilationError>> Analyse(std::ostream& output, bool isBinary);
		
		//函数
		typedef struct function 
		{
			std::string name;
			std::vector<Instruction> instructions;
			int32_t parameter_num;
		}function;

		//符号
		typedef struct symble 
		{
			std::string name;
			bool isConst;
			bool initailized;
		}symble;

	private:
		// 所有的递归子程序

		// <主程序>
		std::optional<CompilationError> analyseProgram();
		// <变量声明>
		std::optional<CompilationError> variable_declaration();
		// <函数定义>
		std::optional<CompilationError> function_definition();
		// <初始化列表>
		std::optional<CompilationError> init_declarator_list(bool flag);
		// <初始化>
		std::optional<CompilationError> init_declarator(bool flag);
		// <函数的参数>
		std::optional<CompilationError> parameter_clause(Token& t);
		// <参数列表>
		std::optional<CompilationError> parameter_declaration_list(Token& t);
		// <参数赋值>
		std::optional<CompilationError> parameter_declaration();
		// <函数调用>
		std::optional<CompilationError> function_call();
		// <调用参数列表>
		std::optional<CompilationError> expression_list(int32_t para_num);
		// <函数组成>
		std::optional<CompilationError> compound_statement();
		// <语句序列>
		std::optional<CompilationError> statement_seq();
		// <语句>
		std::optional<CompilationError> statement();
		// <return语句>
		std::optional<CompilationError> return_statement();
		// <循环语句>
		std::optional<CompilationError> loop_statement();
		// <输入语句>
		std::optional<CompilationError> scan_statement();
		// <输出语句>
		std::optional<CompilationError> print_statement();
		// <输出表>
		std::optional<CompilationError> printable_list();
		// <输出赋值>
		std::optional<CompilationError> printable();
		// <赋值语句>
		std::optional<CompilationError> assignment_expression();
		// <条件>
		std::optional<CompilationError> condition();
		// <表达式>
		std::optional<CompilationError> expression();
		// <乘除表达式>
		std::optional<CompilationError> multiplicative_expression();
		// <单目表达式>
		std::optional<CompilationError> unary_expression();
		// <基础表达式>
		std::optional<CompilationError> primary_expression(int flag);



		
		// Token 缓冲区相关操作

		// 返回下一个 token
		std::optional<Token> nextToken();
		// 回退一个 token
		void unreadToken();
		// 下面是符号表相关操作

		//判断标识符是否是函数
		bool isFunction(Token& tk);
		//是否重复定义
		bool ReDeclare(Token& tk);
		//存入相应的符号表
		bool insertTable(Token& tk, bool isConst, bool initailized);
		//查找偏移，不存在返回-1
		int32_t getOffset(bool global, std::string name);
		//向函数表中增加函数
		bool insertFunction(Token& tk, int32_t par);
		//加入void型，如果函数为void，则不能解析为表达式
		void addVoid(Token& tk);
		//标识符是否是void类型
		bool isVoid(Token& tk);
		//查找函数的参数个数
		int32_t getParnum(Token& tk);
		//查找函数的索引
		int32_t getFunctionIndex(Token& tk);
		//获取符号在相应符号表中的索引
		int32_t getSymbleIndex(Token& tk);
		//查找标识符所在的层级，如果是局部变量则返回1，全局变量则返回0，没有找到则返回-1
		int32_t getLevel(Token& tk);
		//压入对应的指令，如果是全局状态直接压入_instruction,如果是局部状态则压入当前函数的最后一条指令
		bool addInstruction(Instruction instruction);
		//给变量赋值
		bool assign(Token& tk);
		//判断符号是不是常量
		bool isConst(Token& tk);
		//判断符号是否初始化
		bool isInitailized(Token& tk);
		//向跳转指令栈中添加跳转指令的地址
		void pushJump(int32_t);
		//弹出跳转指令栈栈顶的值并返回
		int32_t popJump();
		//输出文本文件
		void printTextFile(std::ostream& output);
		//输出文本指令
		void print_Text(Instruction instruction, std::ostream& output);
		//32位转大端
		void to_binary2(int number, std::ostream& output);
		//16位转大端
		void to_binary4(int number, std::ostream& output);
		//输出二进制文件
		void printBinary(std::ostream& output);
		//输出二进制指令
		void printBinaryInstruction(Instruction instruction, std::ostream& output);
	private:
		std::vector<Token> _tokens;
		std::size_t _offset;
		std::vector<Instruction> _instructions;
		std::pair<uint64_t, uint64_t> _current_pos;
		std::vector<std::string>_void_func;
		std::vector<int32_t>_jump_stack;
		//全局表和局部表
		std::vector<symble> globalTable, localTable;
		std::vector<function> _functions;
		// 下一个 token 在栈的偏移
		int32_t _nextTokenIndex;
		int32_t _functionIndex;
		int32_t ret_num;
		bool isText;
		bool isGlobal;
	};
}
