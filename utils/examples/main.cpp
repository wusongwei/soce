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


extern void cond_expr();
extern void snowflake();
extern void index_list();
extern void group_list();
extern void funnel_buffer();
extern void funnel_queue();
extern void dispatch_queue();
extern void executor();
extern void actor_executor();

int main()
{
    cond_expr();
    snowflake();
    index_list();
    group_list();
    funnel_buffer();
    funnel_queue();
    dispatch_queue();
    executor();
    //actor_executor();

    return 0;
}
