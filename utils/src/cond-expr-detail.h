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

#ifndef _COND_EXPR_DETAIL_utJSGjyw3r_H_
#define _COND_EXPR_DETAIL_utJSGjyw3r_H_

#include "utils/cond-expr.h"

namespace soce{
namespace utils{

    class ExprIf
    {
    public:
        virtual bool calc(ConcreteExpr expr) = 0;
        virtual void show(int indent) = 0;
    };

    class SingleExpr : public ExprIf
    {
    public:
        virtual bool calc(ConcreteExpr expr)
        {
            return expr(lvalue_, operator_, rvalue_);
        }

        inline void set(std::string&& lvalue, std::string&& oper, std::string&& rvalue)
        {
            lvalue_ = std::move(lvalue);
            operator_ = std::move(oper);
            rvalue_ = std::move(rvalue);
        }

        void show(int indent);

    private:
        std::string lvalue_;
        std::string operator_;
        std::string rvalue_;
    };

    class CompExpr : public ExprIf
    {
    public:
        void set_lexpr(std::shared_ptr<ExprIf> expr)
        {
            lexpr_ = expr;
        }

        void set_rexpr(std::shared_ptr<ExprIf> expr)
        {
            rexpr_ = expr;
        }

    protected:
        std::shared_ptr<ExprIf> lexpr_;
        std::shared_ptr<ExprIf> rexpr_;
    };

    class AndExpr : public CompExpr
    {
    public:
        virtual bool calc(ConcreteExpr expr);
        virtual void show(int indent);
    };

    class OrExpr : public CompExpr
    {
    public:
        virtual bool calc(ConcreteExpr expr);
        virtual void show(int indent);
    };

    class SymbolParser
    {
    public:
        void set_data(const char* data, size_t len);
        std::string get_token();
        std::shared_ptr<SingleExpr> get_expr(std::string&& token);
        std::shared_ptr<CompExpr> get_bracket_expr();
        bool end();

    private:
        void skip_space();

    private:
        const char* data_ = NULL;
        size_t len_ = 0;
        size_t pos_ = 0;
    };

} // namespace utils
} // namespace soce

#endif
