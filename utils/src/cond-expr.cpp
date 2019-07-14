/*
* Licensed to the Apache Software Foundation (ASF) under one
* or more contributor license agreements. See the NOTICE file
* distributed with this work for additional information
* regarding copyright ownership. The ASF licenses this file
* to you under the Apache License, Version 2.0 (the
* "License"); you may not use this file except in compliance
* with the License. You may obtain a copy of the License at
*
*   http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing,
* software distributed under the License is distributed on an
* "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
* KIND, either express or implied. See the License for the
* specific language governing permissions and limitations
* under the License.
*/

#include "utils/cond-expr.h"
#include "cond-expr-detail.h"
#include "log4rel/logger.h"

using std::string;

namespace soce{
namespace utils{

    void SingleExpr::show(int indent)
    {
        SOCE_DEBUG << _D(string(indent, ' ')) << lvalue_ << " " << operator_ << " " << rvalue_;
    }

    bool AndExpr::calc(ConcreteExpr expr)
    {
        bool rc = lexpr_ ? lexpr_->calc(expr) : true;
        if (!rc){
            return false;
        }

        return rexpr_ ? rexpr_->calc(expr) : true;
    }

    void AndExpr::show(int indent)
    {
        SOCE_DEBUG << _D(string(indent, ' ')) << "AndExpr elems = " << ((lexpr_ ? 1 : 0) + (rexpr_ ? 1 : 0));

        if (lexpr_){
            lexpr_->show(indent + 4);
        }

        if (rexpr_){
            rexpr_->show(indent + 4);
        }
    }

    bool OrExpr::calc(ConcreteExpr expr)
    {
        bool rc = lexpr_ ? lexpr_->calc(expr) : false;
        if (rc){
            return true;
        }

        return rexpr_ ? rexpr_->calc(expr) : false;
    }

    void OrExpr::show(int indent)
    {
        SOCE_DEBUG << _D(string(indent, ' ')) << "OrExpr elems =" << ((lexpr_ ? 1 : 0) + (rexpr_ ? 1 : 0));

        if (lexpr_){
            lexpr_->show(indent + 4);
        }

        if (rexpr_){
            rexpr_->show(indent + 4);
        }
    }

    void SymbolParser::set_data(const char* data, size_t len)
    {
        data_ = data;
        len_ = len;
    }

    std::string SymbolParser::get_token()
    {
        skip_space();
        if (end()){
            return string();
        }

        size_t beg = pos_;

        if (data_[pos_] == '('){
            ++pos_;
            return "(";
        }
        else if (data_[pos_] == ')'){
            ++pos_;
            return ")";
        }

        while (pos_ < len_){
            if (data_[pos_] == ' '
                || data_[pos_] == '\t'
                || data_[pos_] == '('
                || data_[pos_] == ')'){
                break;
            }
            else{
                ++pos_;
            }
        }

        return std::move(string(data_ + beg, pos_ - beg));
    }

    std::shared_ptr<SingleExpr> SymbolParser::get_expr(std::string&& token)
    {
        std::shared_ptr<SingleExpr> expr;

        string oper = get_token();
        string rvalue = get_token();
        if (!oper.empty() && !rvalue.empty()){
            expr.reset(new SingleExpr);
            expr->set(std::move(token),
                      std::move(oper),
                      std::move(rvalue));
        }

        return expr;
    }

    std::shared_ptr<CompExpr> SymbolParser::get_bracket_expr()
    {
        // 0 : wait for lexpr
        // 1 : wait for oper
        // 2 : wait for rexpr
        int status = 0;
        std::shared_ptr<CompExpr> rc;
        std::shared_ptr<CompExpr> comp_expr;
        std::shared_ptr<ExprIf> lexpr;
        std::shared_ptr<ExprIf> rexpr;

        do{
            if (end()){
                break;
            }

            string token = get_token();
            if (token.empty()){
                return rc;
            }

            if (token == "("){
                if (status == 1){
                    return rc;
                }

                std::shared_ptr<CompExpr> expr = get_bracket_expr();
                if (!expr){
                    return rc;
                }

                if (status == 0){
                    lexpr = expr;
                }
                else{
                    rexpr = expr;
                    if (!comp_expr){
                        return rc;
                    }

                    comp_expr->set_lexpr(lexpr);
                    comp_expr->set_rexpr(rexpr);
                    lexpr = comp_expr;
                    comp_expr.reset();
                }
                status = 1;
            }
            else if (token == "&&"){
                if (status != 1){
                    return rc;
                }
                comp_expr.reset(new AndExpr);
                status = 2;
            }
            else if (token == "||"){
                if (status != 1){
                    return rc;
                }
                comp_expr.reset(new OrExpr);
                status = 2;
            }
            else if(token == ")"){
                break;
            }
            else{
                if (status == 1){
                    return rc;
                }

                std::shared_ptr<SingleExpr> expr = get_expr(std::move(token));
                if (!expr){
                    return rc;
                }

                if (status == 0){
                    lexpr = expr;
                }
                else{
                    rexpr = expr;

                    if (!comp_expr){
                        return rc;
                    }
                    comp_expr->set_lexpr(lexpr);
                    comp_expr->set_rexpr(rexpr);
                    lexpr = comp_expr;
                    comp_expr.reset();
                }

                status = 1;
            }

        }while(true);

        if (comp_expr){
            rc = comp_expr;
        }
        else if (lexpr){
            rc.reset(new AndExpr);
            rc->set_lexpr(lexpr);
        }

        return rc;
    }

    bool SymbolParser::end()
    {
        skip_space();
        return (pos_ >= len_) ? true : false;
    }

    void SymbolParser::skip_space()
    {
        while (pos_ < len_){
            if (data_[pos_] == ' ' || data_[pos_] == '\t'){
                ++pos_;
            }
            else{
                break;
            }
        }
    }

    int CondExpr::parse(const char* data, size_t len)
    {
        SymbolParser parser;
        parser.set_data(data, len);

        // 0 : wait for lexpr
        // 1 : wait for oper
        // 2 : wait for rexpr
        int status = 0;
        std::shared_ptr<CompExpr> comp_expr;
        std::shared_ptr<ExprIf> lexpr;
        std::shared_ptr<ExprIf> rexpr;

        do{
            if (parser.end()){
                break;
            }

            string token = parser.get_token();
            if (token.empty()){
                return -1;
            }

            if (token == "("){
                if (status == 1){
                    return -1;
                }

                std::shared_ptr<CompExpr> expr = parser.get_bracket_expr();
                if (!expr){
                    return -1;
                }

                if (status == 0){
                    lexpr = expr;
                }
                else{
                    rexpr = expr;
                    if (!comp_expr){
                        return -1;
                    }

                    comp_expr->set_lexpr(lexpr);
                    comp_expr->set_rexpr(rexpr);
                    lexpr = comp_expr;
                    comp_expr.reset();
                }
                status = 1;
            }
            else if (token == "&&"){
                if (status != 1){
                    return -1;
                }
                comp_expr.reset(new AndExpr);
                status = 2;
            }
            else if (token == "||"){
                if (status != 1){
                    return -1;
                }
                comp_expr.reset(new OrExpr);
                status = 2;
            }
            else{
                if (status == 1){
                    return -1;
                }

                std::shared_ptr<SingleExpr> expr = parser.get_expr(std::move(token));
                if (!expr){
                    return -1;
                }

                if (status == 0){
                    lexpr = expr;
                }
                else{
                    rexpr = expr;

                    if (!comp_expr){
                        return -1;
                    }
                    comp_expr->set_lexpr(lexpr);
                    comp_expr->set_rexpr(rexpr);
                    lexpr = comp_expr;
                    comp_expr.reset();
                }

                status = 1;
            }

        }while(true);

        if (!comp_expr && !lexpr){
            return -1;
        }
        else if (!comp_expr){
            expr_ = lexpr;
        }
        else{
            expr_ = comp_expr;
        }

        return 0;
    }

    bool CondExpr::calc(ConcreteExpr expr)
    {
        return expr_->calc(expr);
    }

    void CondExpr::show()
    {
        expr_->show(0);
    }

} // namespace utils
} // namespace soce
