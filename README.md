# README

string_matching.cpp：字符串匹配程序源码

实验报告.pdf：实验报告



answer 文件夹：短字符串在长字符串中的精确匹配结果。

> SRR00001_SRR00001_answer.txt：文件 search_SRR00001.txt 中所有短字符串在长字符串SRR00001_rows_ATCG.txt 中的所有精确匹配结果，其中每个短字符串的结果占一行，一共1000行。
>
> SRR311115_SRR163132_answer.txt：文件 search_SRR163132.txt 中所有短字符串在长字符串SRR311115_rows_ATCG.txt中的所有精确匹配结果，其中每个短字符串的结果占一行，一共1000行。

由于只给出了 SRR00001_rows_ATCG.txt 和 SRR311115_rows_ATCG.txt 这两个长串文件，第三个短串文件 search_SRR311115.txt 没有对应的长串进行匹配，因此只有两个匹配答案文件。



长串分治示例 文件夹：对长串SRR311115_rows_ATCG.txt进行预处理后的结果示例。

> SRR311115_rows_ATCG_cut_0.txt：长串SRR311115_rows_ATCG.txt切割出的第一个子串对应的 L 
>
> SRR311115_rows_ATCG_cut_index_0.txt：长串SRR311115_rows_ATCG.txt切割出的第一个子串对应的索引
>
> SRR311115_rows_ATCG_cut_938.txt：长串SRR311115_rows_ATCG.txt切割出的最后一个子串对应的 L 
>
> SRR311115_rows_ATCG_cut_index_938.txt：长串SRR311115_rows_ATCG.txt切割出的最后一个子串对应的索引