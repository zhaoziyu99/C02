#include "analyser.h"
#include <algorithm>
#include <vector>
#include <fstream>
#include <climits>
#include <string>
#include <sstream>
#include <iostream>
#include <cstring>

namespace miniplc0 {
	std::pair<std::vector<Instruction>, std::optional<CompilationError>> Analyser::Analyse(std::ostream& output, bool isBinary)
	{
		isText = false;
		ret_num = 0;
		globalTable.clear();
		localTable.clear();
		auto err = analyseProgram();
		if (err.has_value())
			return std::make_pair(std::vector<Instruction>(), err);
		else
		{
			printTextFile(output);
			return std::make_pair(_instructions, std::optional<CompilationError>());
		}
	}

	// <程序> ::= 'begin'<主过程>'end'
	std::optional<CompilationError> Analyser::analyseProgram()
	{
		// 示例函数，示例如何调用子程序
		auto var = variable_declaration();
		if (var.has_value())
			return var;
		auto fun = function_definition();
		if (fun.has_value())
			return fun;
		return {};
	}

	//<variable - declaration> :: = [<const - qualifier>]<type - specifier> < init - declarator - list>';'

	std::optional<CompilationError> Analyser::variable_declaration()
	{
		while (true)
		{
			auto flag = false;  //用于判断常量还是变量
			auto var = nextToken();
			if (!var.has_value())
				return {};
			if (var.value().GetType() == TokenType::LEFT_BRACE
				|| var.value().GetType() == TokenType::IF
				|| var.value().GetType() == TokenType::WHILE
				|| var.value().GetType() == TokenType::RETURN
				|| var.value().GetType() == TokenType::PRINT
				|| var.value().GetType() == TokenType::SCAN
				|| var.value().GetType() == TokenType::IDENTIFIER
				|| var.value().GetType() == TokenType::SEMICOLON)	//读到了statement
			{
				unreadToken();
				return {};
			}
			if (var.value().GetType() == TokenType::CONST)		//读到了const声明，将flag置为true
			{
				flag = true;
				var = nextToken();
			}
			if (var.value().GetType() == TokenType::INT)
			{
				auto next = nextToken();
				if(!next.has_value())
					return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidVariableDeclaration);
				if (next.value().GetType() == TokenType::IDENTIFIER)
				{
					next = nextToken();
					if (next.value().GetType() == TokenType::LEFT_BRACKET)  //读到的其实是函数声明
					{
						unreadToken();
						unreadToken();
						unreadToken();
						if (flag)
							unreadToken();
						return {};
					}
					else
					{
						unreadToken();
						unreadToken();
						auto err = init_declarator_list(flag);
						if (err.has_value())
							return err;
						next = nextToken();
						if (!next.has_value() || next.value().GetType() != TokenType::SEMICOLON)
							return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
					}
				}
				else
					return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidIdentifier);
			}
			else if (var.value().GetType() == TokenType::VOID)
			{
				if(flag)    //声明了const void
					return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrVoidVariableDeclaration);
				else
				{
					auto next = nextToken();
					if(!next.has_value())
						return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidVariableDeclaration);
					if (next.value().GetType() == TokenType::IDENTIFIER)
					{
						auto next = nextToken();
						if (next.value().GetType() == TokenType::LEFT_BRACKET)   //声明的是void类型的函数
						{
							unreadToken();
							unreadToken();
							unreadToken();
							return {};
						}
						else
							return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrVoidVariableDeclaration);
					}
					else
						return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidIdentifier);
				}
			}
			else if (!flag)
			{
				unreadToken();
				return {};
			}
			else
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidVariableDeclaration);
		}
	}

	//< init - declarator - list > :: = <init - declarator>{ ',' < init - declarator > }
	std::optional<CompilationError> Analyser::init_declarator_list(bool flag) 
	{
		auto dec = init_declarator(flag);
		if (dec.has_value())
			return dec;
		while (1)
		{
			auto err = nextToken();  //下一个符号要么是逗号要么是分号
			if (!err.has_value())
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
			if (err.value().GetType() == TokenType::COMMA)
			{
				auto dec2 = init_declarator(flag);
				if (dec2.has_value())
					return dec2;
			}
			else
			{
				unreadToken();
				return {};
			}
		}
	}

	//<init - declarator> :: = <identifier>[<initializer>]
	//<initializer> :: = '=' < expression >
	std::optional<CompilationError> Analyser::init_declarator(bool flag)  
	{
		auto id = nextToken();
		if (!id.has_value() || id.value().GetType() != TokenType::IDENTIFIER)		//标识符
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidIdentifier);
	
		auto next = nextToken();
		if (flag)	//flag=true,声明的是const常量
		{
			if (!next.has_value() || next.value().GetType() != TokenType::ASSIGN_SIGN)		//赋值用的单等号'='
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrConstantNeedValue);   //常量未赋值
			else
			{
				if(!insertTable(id.value(), true, true))
					return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrDuplicateDeclaration);
				auto exp = expression();
				if (exp.has_value())
					return exp;
			}
		}
		else	//声明的不是const
		{
			if(!next.has_value())
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidVariableDeclaration);
			if (next.value().GetType() != TokenType::ASSIGN_SIGN)  //未赋值的int变量
			{
				if(!insertTable(id.value(), false, false))
					return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrDuplicateDeclaration);
				addInstruction(Instruction(ipush, 0));
				unreadToken();
			}
			else	//有赋值的int
			{
				if(!insertTable(id.value(), false, true))
					return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrDuplicateDeclaration);
				auto exp = expression();
				if (exp.has_value())
					return exp;
			}
		}
		return {};
	}

	//<function - definition> :: = <type - specifier><identifier><parameter - clause><compound - statement>
	std::optional<CompilationError> Analyser::function_definition()
	{
		while (true)
		{
			isGlobal = false;	//此时不是全局，优先使用局部表
			localTable.clear();
			bool retn = true;//记录函数是否需要返回
			ret_num = 0;//函数参数个数
			auto type = nextToken();
			if (!type.has_value())
				return {};
			if (type.value().GetType() == TokenType::INT)
				retn = true;
			if (type.value().GetType() == TokenType::VOID)
				retn = false;
			if (type.value().GetType() != TokenType::INT && type.value().GetType() != TokenType::VOID)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidFunctionDefinition);
			auto id = nextToken();
			if (!id.has_value() || id.value().GetType() != TokenType::IDENTIFIER)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidIdentifier);

			if (!retn)  //无返回值则将该函数加入无返回中
				addVoid(id.value());

			auto err = parameter_clause(id.value());	//<parameter - clause>
			if (err.has_value())
				return err;
			auto cmp = compound_statement();		//<compound - statement>
			if (cmp.has_value())
				return cmp;

			if(retn && ret_num == 0)   //需要return但是实际未返回参数
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoReturn);
			else if(!retn && ret_num > 0)  //无返回函数返回了参数
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoNeedReturn);

			addInstruction(Instruction(ret));
			isGlobal = true;	//再次切换回全局状态
			localTable.clear();
		}
		return {};
	}

	//<parameter - clause> :: = '('[<parameter - declaration - list>] ')'
	std::optional<CompilationError> Analyser::parameter_clause(Token& t)
	{
		auto lfb = nextToken();
		if (!lfb.has_value() || lfb.value().GetType() != TokenType::LEFT_BRACKET)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteBracket);
		auto rtb = nextToken();
		if (!rtb.has_value())
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteBracket);
		if (rtb.value().GetType() != TokenType::RIGHT_BRACKET)
		{
			unreadToken();
			auto err = parameter_declaration_list(t);
			if (err.has_value())
				return err;
			auto rt = nextToken();
			if (!rt.has_value() || rt.value().GetType() != TokenType::RIGHT_BRACKET)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteBracket);
			return {};
		}
		else  //读入的为有括号，没有参数声明
		{
			if(!insertFunction(t, 0))//添加函数到表中,参数为0个
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrDuplicateDeclaration);
			return {};
		}
	}

	//< parameter - declaration - list > :: = <parameter - declaration>{ ',' < parameter - declaration > }
	std::optional<CompilationError> Analyser::parameter_declaration_list(Token& t)
	{
		int32_t para_num = 1;
		auto err = parameter_declaration();
		if (err.has_value())
			return err;
		while (true)
		{
			auto comma = nextToken();
			if (!comma.has_value())
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteBracket);
			if (comma.value().GetType() != TokenType::COMMA)
			{
				unreadToken();	//下一个字符不是","，回退
				if(!insertFunction(t, para_num))	//将函数加入函数表
					return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrDuplicateDeclaration);
				return {};
			}
			else
			{
				auto err2 = parameter_declaration();
				if (err2.has_value())
					return err2;
				para_num++;
			}
		}
	}

	//<parameter - declaration> :: = [<const - qualifier>]<type - specifier><identifier>
	std::optional<CompilationError> Analyser::parameter_declaration()
	{
		bool cst = false;//记录参数是否为const类型
		auto next = nextToken();
		if (!next.has_value() || (next.value().GetType() != TokenType::CONST && next.value().GetType() != TokenType::INT && next.value().GetType() != TokenType::VOID))
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidParameterDeclaration);
		else if (next.value().GetType() == TokenType::CONST)
		{
			cst = true;
			auto type = nextToken();
			if (!type.has_value() || (type.value().GetType() != TokenType::INT && type.value().GetType() != TokenType::VOID))
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidParameterDeclaration);
			if(type.value().GetType() != TokenType::VOID)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrVoidVariableDeclaration);
		}
		if(next.value().GetType() == TokenType::VOID)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrVoidVariableDeclaration);
		auto id = nextToken();
		if (!id.has_value() || id.value().GetType() != TokenType::IDENTIFIER)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidFunctionDefinition);
		//有参数的函数传参操作
		insertTable(id.value(), cst, true);
		return {};
	}

	//<function - call> :: = < identifier> '('[<expression - list>] ')'
	std::optional<CompilationError> Analyser::function_call()
	{
		auto next = nextToken();
		if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNotFunctionCall);
		int32_t offset = getFunctionIndex(next.value());
		if(offset == -1)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNotDeclared);
		auto func = next.value();
		next = nextToken();
		if (!next.has_value() || next.value().GetType() != TokenType::LEFT_BRACKET)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteBracket);
		next = nextToken();
		if (!next.has_value())
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteBracket);
		if (next.value().GetType() == TokenType::RIGHT_BRACKET)
		{
			if(getParnum(func) > 0)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInsufficientArguments);
			addInstruction(Instruction(call, offset));
			return {};
		}

		unreadToken();
		auto err = expression_list(getParnum(func));
		if (err.has_value())
			return err;

		auto next2 = nextToken();
		if (!next2.has_value() || next2.value().GetType() != TokenType::RIGHT_BRACKET)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteBracket);
		addInstruction(Instruction(call, offset));
		return {};
	}

	//< expression - list > :: = <expression>{ ',' < expression > }
	std::optional<CompilationError> Analyser::expression_list(int32_t para_num)
	{
		int count = 1;
		auto err = expression();
		if (err.has_value())
			return err;
		while (true)
		{
			auto next = nextToken();
			if (!next.has_value())
				return {};
			if (next.value().GetType() != TokenType::COMMA)
			{
				unreadToken();
				if(count>para_num)
					return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrExcessiveArguments);
				else if(count<para_num)
					return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInsufficientArguments);
				return {};
			}
			else
			{
				count++;
				auto err = expression();
				if (err.has_value())
					return err;
			}
		}
	}

	//<compound - statement> :: = '{' {<variable - declaration>} < statement - seq> '}'
	std::optional<CompilationError> Analyser::compound_statement()
{
		auto next = nextToken();
		if (!next.has_value() || next.value().GetType() != TokenType::LEFT_BRACE)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteBrace);
		auto var = variable_declaration();
		if (var.has_value())
			return var;
		auto stat = statement_seq();
		if (stat.has_value())
			return stat;
		auto next2 = nextToken();
		if (!next2.has_value() || next2.value().GetType() != TokenType::RIGHT_BRACE)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteBrace);
		return {};
	}

	//<statement - seq> :: = { <statement> }
	std::optional<CompilationError> Analyser::statement_seq()
	{
		while (true)
		{
			auto next = nextToken();
			if (!next.has_value())
				return {};
			unreadToken();
			if (next.value().GetType() != TokenType::LEFT_BRACE
				&& next.value().GetType() != TokenType::IF
				&& next.value().GetType() != TokenType::WHILE
				&& next.value().GetType() != TokenType::RETURN
				&& next.value().GetType() != TokenType::PRINT
				&& next.value().GetType() != TokenType::SCAN
				&& next.value().GetType() != TokenType::IDENTIFIER
				&& next.value().GetType() != TokenType::SEMICOLON)
				return{};
			auto stat = statement();
			if (stat.has_value())
				return stat;
		}
		return {};
	}

	/*<statement> :: =
		'{' < statement - seq > '}'
		| <condition - statement>
		| <loop - statement>
		| <jump - statement>
		| <print - statement>
		| <scan - statement>
		| < assignment - expression>';'
		| < function - call>';'
		| ';'*/
	std::optional<CompilationError> Analyser::statement()
	{
		auto next = nextToken();
		if (!next.has_value())
			return {};
		if (next.value().GetType() == TokenType::LEFT_BRACE)    //statement seq
		{
			auto stat = statement_seq();
			if (stat.has_value())
				return stat;
			auto next2 = nextToken();
			if (!next2.has_value() || next2.value().GetType() != TokenType::RIGHT_BRACE)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteBrace);
			return {};
		}
		//<condition-statement> ::= 'if' '(' < condition > ')' < statement > ['else' < statement > ]
		else if (next.value().GetType() == TokenType::IF)
		{
			auto lft = nextToken();
			if (!lft.has_value() || lft.value().GetType() != TokenType::LEFT_BRACKET)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteBracket);

			auto con = condition();
			if (con.has_value())
				return con;

			auto rit = nextToken();
			if (!rit.has_value() || rit.value().GetType() != TokenType::RIGHT_BRACKET)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteBracket);

			auto stat = statement();
			if (stat.has_value())
				return stat;

			auto next3 = nextToken();
			if (!next3.has_value() || next3.value().GetType() != TokenType::ELSE)
			{
				unreadToken();
				_functions.back().instructions[popJump()].SetX(_functions.back().instructions.size());
				addInstruction(Instruction(nop));
				return{};
			}
			else
			{
				_functions.back().instructions[popJump()].SetX(_functions.back().instructions.size() + 1);
				addInstruction(Instruction(jmp));
				int32_t offset = _functions.back().instructions.size() - 1;//记住jmp指令的地址
				auto stat = statement();
				if (stat.has_value())
					return stat;
				_functions.back().instructions[offset].SetX(_functions.back().instructions.size());
				addInstruction(Instruction(nop));
				return {};
			}
		}
		else if (next.value().GetType() == TokenType::WHILE)   //<loop-statement> ::= 'while' '(' < condition > ')' < statement >
		{
			auto next2 = nextToken();
			if (!next2.has_value() || next2.value().GetType() != TokenType::LEFT_BRACKET)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteBracket);
			pushJump(_functions.back().instructions.size());
			auto con = condition();
			if (con.has_value())
				return con;

			auto next3 = nextToken();
			if (!next3.has_value() || next3.value().GetType() != TokenType::RIGHT_BRACKET)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteBracket);

			auto stat = statement();
			if (stat.has_value())
				return stat;

			_functions.back().instructions[popJump()].SetX(_functions.back().instructions.size() + 1);
			addInstruction(Instruction(jmp, popJump()));
			addInstruction(Instruction(nop));
			return {};
		}
		else if (next.value().GetType() == TokenType::RETURN) //<return-statement> ::= 'return' [<expression>] ';'
		{
			auto next2 = nextToken();
			if (!next2.has_value())
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
			if (next2.value().GetType() != TokenType::SEMICOLON)
			{
				unreadToken();
				auto expr = expression();
				if (expr.has_value())
					return expr;
				auto next3 = nextToken();
				if (!next3.has_value() || next3.value().GetType() != TokenType::SEMICOLON)
					return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
				ret_num++;
				addInstruction(Instruction(iret));
			}
			addInstruction(Instruction(ret));
			return {};
		}
		else if (next.value().GetType() == TokenType::SCAN)  //<scan-statement>  ::= 'scan' '(' <identifier> ')' ';'
		{
			auto next2 = nextToken();
			if (!next2.has_value() || next2.value().GetType() != TokenType::LEFT_BRACKET)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteBracket);
			auto id = nextToken();
			if (!id.has_value() || id.value().GetType() != TokenType::IDENTIFIER)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidScanStatement);

			int32_t offset = getSymbleIndex(id.value());
			if(getLevel(id.value()) == -1) //变量未声明
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNotDeclared);
			else if (getLevel(id.value()) == 0) //全局变量
				addInstruction(Instruction(loada, 1, offset));
			else if (getLevel(id.value()) == 1)
				addInstruction(Instruction(loada, 0, offset));
			assign(id.value());
			addInstruction(Instruction(iscan));
			addInstruction(Instruction(istore));


			auto next3 = nextToken();
			if (!next3.has_value() || next3.value().GetType() != TokenType::RIGHT_BRACKET)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidScanStatement);
			auto next4 = nextToken();
			if (!next4.has_value() || next4.value().GetType() != TokenType::SEMICOLON)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
			return {};
		}
		else if (next.value().GetType() == TokenType::PRINT) //< print - statement > :: = 'print' '('[<printable - list>] ')' ';'
		{
			auto next2 = nextToken();
			if (!next2.has_value() || next2.value().GetType() != TokenType::LEFT_BRACKET)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidPrintStatement);
			auto next3 = nextToken();
			if (!next3.has_value())
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidPrintStatement);
			if (next3.value().GetType() != TokenType::RIGHT_BRACKET)
			{
				unreadToken();
				auto prt = printable_list();
				if (prt.has_value())
					return prt;

				auto next4 = nextToken();
				if (!next4.has_value() || next4.value().GetType() != TokenType::RIGHT_BRACKET)
					return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteBracket);
			}

			auto next4 = nextToken();
			if (!next4.has_value() || next4.value().GetType() != TokenType::SEMICOLON)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
			addInstruction(Instruction(printl));
			return {};
		}
		else if (next.value().GetType() == TokenType::SEMICOLON)
			return {};
		else if (next.value().GetType() == TokenType::IDENTIFIER)
		{
			auto next2 = nextToken();
			if (!next2.has_value())
				return {};
			unreadToken();
			unreadToken();
			if (next2.value().GetType() == TokenType::LEFT_BRACKET)
			{
				auto func = function_call();
				if (func.has_value())
					return func;
			}
			else if(next2.value().GetType() == TokenType::ASSIGN_SIGN)
			{
				auto asg = assignment_expression();
				if (asg.has_value())
					return asg;
			}
			auto semi = nextToken();
			if (semi.value().GetType() != SEMICOLON)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
			return {};
		}
		else
		{
			unreadToken();
			return {};
		}
	}

	//<printable - list>  :: = <printable>{ ',' < printable > }
	std::optional<CompilationError> Analyser::printable_list()
	{
		auto exp = expression();
		if (exp.has_value())
			return exp;
		addInstruction(Instruction(iprint));
		while (true)
		{
			auto next = nextToken();
			if (!next.has_value())
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteBracket);
			if (next.value().GetType() == TokenType::COMMA)
			{
				auto exp = expression();
				if (exp.has_value())
					return exp;
			}
			else
			{
				unreadToken();
				return {};
			}
			addInstruction(Instruction(bipush, 32));
			addInstruction(Instruction(cprint));
			addInstruction(Instruction(iprint));
		}

	}

	//<assignment - expression> :: = <identifier><assignment - operator><expression>
	std::optional<CompilationError> Analyser::assignment_expression()
	{
		auto next = nextToken();
		if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidAssignment);
		int32_t offset = getSymbleIndex(next.value());
		if(getLevel(next.value()) == 1) //局部变量
			addInstruction(Instruction(loada, 0, offset));
		else if (getLevel(next.value()) == 0) // 全局变量
			addInstruction(Instruction(loada, 1, offset));
		else if (getLevel(next.value()) == -1)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNotDeclared);
		if(isConst(next.value()))	//是否声明为常量
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrAssignToConstant);
		if (!isInitailized(next.value()))	//之前是否初始化过
			assign(next.value());

		auto next2 = nextToken();
		if (!next2.has_value() || next2.value().GetType() != TokenType::ASSIGN_SIGN)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidAssignment);
		auto exp = expression();
		if (exp.has_value())
			return exp;

		addInstruction(Instruction(istore));
		return {};
	}

	//condition> ::= expression>[<relational - operator><expression>]
	std::optional<CompilationError> Analyser::condition()
	{
		auto exp = expression();
		if (exp.has_value())
			return exp;
		auto next = nextToken();
		if (!next.has_value())
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteBracket);
		if (next.value().GetType() == TokenType::RIGHT_BRACKET)
		{
			addInstruction(Instruction(je));
			pushJump(_functions.back().instructions.size() - 1);
			unreadToken();
			return {};
		}
		else if(next.value().GetType() != TokenType::MORETHAN
			&& next.value().GetType() != TokenType::LESSTHAN
			&& next.value().GetType() != TokenType::NOLESSTHAN
			&& next.value().GetType() != TokenType::NOMORETHAN
			&& next.value().GetType() != TokenType::NOT_EQUAL_SIGN
			&& next.value().GetType() != TokenType::EQUAL_SIGN)		//不是比较符号
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidConditionStatement);
		auto err = expression();
		if (err.has_value())
			return err;

		addInstruction(Instruction(icmp));

		if (next.value().GetType() == TokenType::EQUAL_SIGN)
			addInstruction(Instruction(jne));
		else if (next.value().GetType() == TokenType::NOLESSTHAN)
			addInstruction(Instruction(jl));
		else if (next.value().GetType() == TokenType::MORETHAN)
			addInstruction(Instruction(jle));
		else if (next.value().GetType() == TokenType::NOMORETHAN)
			addInstruction(Instruction(jg));
		else if (next.value().GetType() == TokenType::LESSTHAN)
			addInstruction(Instruction(jge));
		else if (next.value().GetType() == TokenType::NOT_EQUAL_SIGN)
			addInstruction(Instruction(je));
		else
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidConditionStatement);
		pushJump(_functions.back().instructions.size() - 1);
		return {};
	}


	//<expression> :: = <additive - expression>
	//<additive - expression> :: = <multiplicative - expression>{ <additive - operator><multiplicative - expression> }
	std::optional<CompilationError> Analyser::expression()
	{
		auto add = multiplicative_expression();
		if (add.has_value())
			return add;
		while (true)
		{
			auto next = nextToken();
			if (!next.has_value())
				return {};
			if (next.value().GetType() == TokenType::PLUS_SIGN || next.value().GetType() == TokenType::MINUS_SIGN)
			{
				auto mul = multiplicative_expression();
				if (mul.has_value())
					return mul;
			}
			else
			{
				unreadToken();
				return {};
			}
			if(next.value().GetType() == TokenType::PLUS_SIGN)
				addInstruction(Instruction(iadd));
			if(next.value().GetType() == TokenType::MINUS_SIGN)
				addInstruction(Instruction(isub));
		}
	}

	//<multiplicative - expression> :: = <unary - expression>{ <multiplicative - operator><unary - expression> }
	std::optional<CompilationError> Analyser::multiplicative_expression()
	{
		auto uny = unary_expression();
		if (uny.has_value())
			return uny;
		while (true)
		{
			auto next = nextToken();
			if (!next.has_value())
				return {};
			if (next.value().GetType() == TokenType::MULTIPLICATION_SIGN || next.value().GetType() == TokenType::DIVISION_SIGN)
			{
				auto una = unary_expression();
				if (una.has_value())
					return una;
			}
			else
			{
				unreadToken();
				return {};
			}
			if(next.value().GetType() == TokenType::MULTIPLICATION_SIGN)
				addInstruction(Instruction(imul));
			if(next.value().GetType() == TokenType::DIVISION_SIGN)
				addInstruction(Instruction(idiv));
		}
	}

	//<unary - expression> :: = [<unary - operator>]<primary - expression>
	std::optional<CompilationError> Analyser::unary_expression()
	{
		int32_t flag = 1;
		auto next = nextToken();
		if(!next.has_value())
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidExpression);
		if (next.value().GetType() == TokenType::PLUS_SIGN)
			flag = 1;
		else if (next.value().GetType() == TokenType::MINUS_SIGN)
			flag = -1;
		else
			unreadToken();	//无加号默认为正，flag = 1
		auto pri = primary_expression(flag);
		if (pri.has_value())
			return pri;
		return {};
	}

	//<primary - expression> :: = '(' < expression > ')' | <identifier> | <integer - literal> | <function - call>
	std::optional<CompilationError> Analyser::primary_expression(int flag)
	{
		auto next = nextToken();
		if (!next.has_value())
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidExpression);
		switch (next.value().GetType())
		{
		case TokenType::LEFT_BRACKET:
		{
			auto exp = expression();
			if (exp.has_value())
				return exp;
			auto next = nextToken();
			if (!next.has_value() || next.value().GetType() != TokenType::RIGHT_BRACKET)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidExpression);
			break;
		}
		case TokenType::DECIMAL:
		{
			addInstruction(Instruction(ipush, std::stol(next.value().GetValueString())));
			break;
		}
		case TokenType::HEXADECIMAL:
		{
			addInstruction(Instruction(ipush, std::stol(next.value().GetValueString())));
			break;
		}
		case TokenType::IDENTIFIER:
		{
			if (isVoid(next.value()))
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrVoidExpression);
			auto next2 = nextToken();
			if (!next2.has_value())
				return {};
			if (next2.value().GetType() == TokenType::LEFT_BRACKET)
			{
				unreadToken();
				unreadToken();
				auto func = function_call();
				if (func.has_value())
					return func;
				return {};
			}
			else
			{
				unreadToken();
				int32_t offset = getSymbleIndex(next.value());
				switch (getLevel(next.value()))		//根据标识符的层级来查找
				{
					case 0:		 //全局变量
					{
						if (!isInitailized(next.value()))
							return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNotInitialized);
						if(isGlobal)
							addInstruction(Instruction(loada, 0, offset));
						else
							addInstruction(Instruction(loada, 1, offset));
						break;
					}
					case 1:		//局部变量
					{
						if(!isInitailized(next.value()))
							return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNotInitialized);
						addInstruction(Instruction(loada, 0, offset));
						break;
					}
					case -1:
						return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNotDeclared);
				}
				addInstruction(Instruction(iload));
			}
			break;
		}
		default:
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrInvalidExpression);
		}
		if(flag == -1)
			addInstruction(Instruction(ineg));
		return {};
	}

	std::optional<Token> Analyser::nextToken() {
		if (_offset == _tokens.size())
			return {};
		// 考虑到 _tokens[0..._offset-1] 已经被分析过了
		// 所以我们选择 _tokens[0..._offset-1] 的 EndPos 作为当前位置
		_current_pos = _tokens[_offset].GetEndPos();
		return _tokens[_offset++];
	}

	void Analyser::unreadToken() {
		if (_offset == 0)
			DieAndPrint("analyser unreads token from the begining.");
		_current_pos = _tokens[_offset - 1].GetEndPos();
		_offset--;
	}


	//判断标识符是否是函数
	bool Analyser::isFunction(Token& tk) {
		for (int i = 0; i < _functions.size(); i++) {
			if (_functions[i].name == tk.GetValueString())
				return true;
		}
		return false;
	}

	//加入void型，如果函数位void型则不能解析为表达式
	void Analyser::addVoid(Token& tk) {
		_void_func.push_back(tk.GetValueString());
	}
	//标识符是否是void类型
	bool Analyser::isVoid(Token& tk) {
		for (int32_t i = 0; i < _void_func.size(); i++) {
			if (tk.GetValueString() == _void_func[i])
				return true;
		}
		return false;
	}

	int32_t Analyser::getOffset(bool global, std::string name)
	{
		if (global)//标记在全局还是局部表中查找
		{
			for (int i = 0; i < globalTable.size(); i++)
			{
				if (globalTable[i].name == name)//此时范围为全局，在全局表中查找是否重复定义
					return i;
			}
			return -1;
		}
		else
		{
			for (int i = 0; i < localTable.size(); i++)
			{
				if (localTable[i].name == name)//此时范围为局部，在局部表中查找是否重复定义
					return i;
			}
			return -1;
		}
	}

	//向对应的符号表中插入符号，函数例外，如果符号重复声明则返回false
	bool Analyser::insertTable(Token& t, bool isConst, bool initailized) 
	{
		if (isGlobal)	//全局表
		{
			if (getOffset(true, t.GetValueString()) != -1)
				return false;
			symble s;	//将新的变量插入表中
			s.name = t.GetValueString();
			s.isConst = isConst;
			s.initailized = initailized;
			globalTable.push_back(s);
		}
		else			//局部表
		{
			if (getOffset(false, t.GetValueString()) != -1)
				return false;
			symble s;
			s.name = t.GetValueString();
			s.isConst = isConst;
			s.initailized = initailized;
			localTable.push_back(s);
		}
		return true;
	}
	//向函数表中函数，如果符号重复声明则返回false
	bool Analyser::insertFunction(Token& tk, int32_t par) 
	{
		if (isFunction(tk))
			return false;
		//检查重复定义
		if (getOffset(true, tk.GetValueString()) != -1)
			return false;

		function newFun;
		newFun.instructions.clear();
		newFun.name = tk.GetValueString();
		newFun.parameter_num = par;
		_functions.push_back(newFun);
		return true;
	}
	//查找函数的参数个数
	int32_t Analyser::getParnum(Token& tk) 
	{
		for (int32_t i = 0; i < _functions.size(); i++) 
		{
			if (tk.GetValueString() == _functions[i].name)
				return _functions[i].parameter_num;
		}
		return -1;
	}
	//返回函数的索引
	int32_t Analyser::getFunctionIndex(Token& tk) 
	{
		for (int32_t i = 0; i < _functions.size(); i++) 
		{
			if (tk.GetValueString() == _functions[i].name)
				return i;
		}
		return -1;
	}
	//获取符号在相应符号表中的索引
	int32_t Analyser::getSymbleIndex(Token& tk) 
	{
		if (getLevel(tk) == 1)
			return getOffset(false, tk.GetValueString());
		else if (getLevel(tk) == 0)
			return getOffset(true, tk.GetValueString());
		else
			return -1;
	}
	//查找标识符所在的层级，如果是局部变量则返回1，全局变量则返回0，没有找到则返回-1
	int32_t Analyser::getLevel(Token& tk) 
	{
		if (!isGlobal) 
		{
			if (getOffset(false, tk.GetValueString()) != -1)
				return 1;
		}
		if (getOffset(true, tk.GetValueString()) != -1)
			return 0;
		return -1;
	}
	//压入对应的指令，如果是全局状态直接压入_instruction,如果是局部状态则压入当前函数的最后一条指令
	bool Analyser::addInstruction(Instruction instruction) 
	{
		if (isGlobal)
			_instructions.push_back(instruction);
		else 
		{
			if (_functions.size() <= 0)
				return false;
			else
				_functions.back().instructions.push_back(instruction);
		}
		return true;
	}
	//给变量赋值,改变initailize属性
	bool Analyser::assign(Token& tk) 
	{
		int32_t index = getSymbleIndex(tk);
		if (getLevel(tk) == 1) 
		{
			localTable[index].initailized = true;
			return true;
		}
		else if (getLevel(tk) == 0) 
		{
			globalTable[index].initailized = true;
			return true;
		}
		else
			return false;
	}
	//判断符号是不是常量
	bool Analyser::isConst(Token& tk) 
	{
		int32_t index = getSymbleIndex(tk);
		if (getLevel(tk) == 1)
			return localTable[index].isConst;
		else if (getLevel(tk) == 0)
			return globalTable[index].isConst;
		else 
			return false;
	}
	//判断符号是否初始化
	bool Analyser::isInitailized(Token& tk) 
	{
		int32_t index = getSymbleIndex(tk);
		if (getLevel(tk) == 1)
			return localTable[index].initailized;
		else if (getLevel(tk) == 0)
			return globalTable[index].initailized;
		else
			return false;
	}

	//向跳转指令栈中添加跳转指令的地址
	void Analyser::pushJump(int32_t jump) 
	{
		_jump_stack.push_back(jump);
	}

	//弹出跳转指令栈栈顶的值并返回
	int32_t Analyser::popJump() 
	{
		int32_t result = _jump_stack.back();
		_jump_stack.pop_back();
		return result;
	}

	//输出文本文件
	void Analyser::printTextFile(std::ostream& output) 
	{
		output << ".constants:" << std::endl;
		for (int32_t i = 0; i < _functions.size(); i++)
			output << i << " " << 'S' << " \"" << _functions[i].name << "\"" << std::endl;
		output << "." << "start" << ":" << std::endl;
		for (int32_t i = 0; i < _instructions.size(); i++) 
		{
			output << i << " ";
			print_Text(_instructions[i], output);
		}
		output << ".functions:" << std::endl;
		for (int32_t i = 0; i < _functions.size(); i++)
			output << i << " " << i << " " << _functions[i].parameter_num << " " << "1" << std::endl;
		for (int32_t i = 0; i < _functions.size(); i++) 
		{
			output << "." << "F" << i << ":" << std::endl;
			for (int32_t j = 0; j < _functions[i].instructions.size(); j++) 
			{
				output << j << " ";
				print_Text(_functions[i].instructions[j], output);
			}
		}
	}

	//输出文本指令
	void Analyser::print_Text(Instruction instruction, std::ostream& output) 
	{
		std::string name;
		switch (instruction.GetOperation())
		{
			case miniplc0::bipush: 
			{
				name = "bipush";
				break;
			}
			case miniplc0::ipush: 
			{
				name = "ipush";
				break;
			}
			case miniplc0::pop: 
			{
				name = "pop";
				break;
			}
			case miniplc0::loadc: 
			{
				name = "loadc";
				break;
			}
			case miniplc0::loada: 
			{
				name = "loada";
				//	std::cout << "Y=" << instruction.GetY() << std::endl;
				break;
			}
			case miniplc0::iload: 
			{
				name = "iload";
				break;
			}
			case miniplc0::aload: 
			{
				name = "aload";
				break;
			}
			case miniplc0::iaload: 
			{
				name = "iaload";
				break;
			}
			case miniplc0::istore: 
			{
				name = "istore";
				break;
			}
			case miniplc0::iastore: 
			{
				name = "iastore";
				break;
			}
			case miniplc0::iadd: 
			{
				name = "iadd";
				break;
			}
			case miniplc0::isub: 
			{
				name = "isub";
				break;
			}
			case miniplc0::imul: 
			{
				name = "imul";
				break;
			}
			case miniplc0::idiv: 
			{
				name = "idiv";
				break;
			}
			case miniplc0::ineg: 
			{
				name = "ineg";
				break;
			}
			case miniplc0::icmp: 
			{
				name = "icmp";
				break;
			}
			case miniplc0::jmp: 
			{
				name = "jmp";
				break;
			}
			case miniplc0::je: 
			{
				name = "je";
				break;
			}
			case miniplc0::jne: 
			{
				name = "jne";
				break;
			}
			case miniplc0::jl: 
			{
				name = "jl";
				break;
			}
			case miniplc0::jle: 
			{
				name = "jle";
				break;
			}
			case miniplc0::jg: 
			{
				name = "jg";
				break;
			}
			case miniplc0::jge: 
			{
				name = "jge";
				break;
			}
			case miniplc0::call: 
			{
				name = "call";
				break;
			}
			case miniplc0::ret: 
			{
				name = "ret";
				break;
			}
			case miniplc0::iret: 
			{
				name = "iret";
				break;
			}
			case miniplc0::iprint: 
			{
				name = "iprint";
				break;
			}
			case miniplc0::cprint: 
			{
				name = "cprint";
				break;
			}
			case miniplc0::printl: 
			{
				name = "printl";
				break;
			}
			case miniplc0::iscan: 
			{
				name = "iscan";
				break;
			}
			case miniplc0::nop: 
			{
				name = "nop";
				break;
			}
			default:
				name = "error";
				break;
		}
		output << name;
		if (instruction.GetX() >= 0)
			output << " " << instruction.GetX();
		if (instruction.GetY() >= 0)
			output << ", " << instruction.GetY();
		output << std::endl;
	}

	//大端法转换
	void Analyser::to_binary4(int number, std::ostream& output) {
		char buffer[4];
		buffer[0] = ((number & 0xff000000) >> 24);
		buffer[1] = ((number & 0x00ff0000) >> 16);
		buffer[2] = ((number & 0x0000ff00) >> 8);
		buffer[3] = ((number & 0x000000ff));
		output.write(buffer, sizeof(buffer));
	}
	void Analyser::to_binary2(int number, std::ostream& output) {
		char buffer[2];
		buffer[0] = ((number & 0x0000ff00) >> 8);
		buffer[1] = ((number & 0x000000ff));
		output.write(buffer, sizeof(buffer));
	}
}