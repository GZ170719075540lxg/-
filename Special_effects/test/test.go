package main
import "fmt"

/**
 * 代码中的类名、方法名、参数名已经指定，请勿修改，直接返回方法规定的值即可
 *
 * 
 * @param strs string字符串一维数组 
 * @return string字符串
*/
func longestCommonPrefix( strs []string ) string {
    // write code here
	len := 0
	for i := 0;; i++ {
		 flag := true
		for j := 0; j < strs.length; j++ {
			if(strs[j][i]==strs[j+1][i]){
				flag = false
			}
		}
		if(flag){
			break;
		}
		len++
	}
	return strs[0:len]
}