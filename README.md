這是用C寫的霍夫曼壓縮
功能:
 1. 將檔案放在各自的資料夾
 2. 輸入檔名就可以壓縮或解壓縮

壓縮:
 1. 我將檔案以byte的方式讀入，統計256種byte的數量後
 2. 建立霍夫曼樹
 3. 在壓縮檔檔案中前面先記住這個檔案的副檔名
 4. 再將霍夫曼樹根據DFS的順序寫進檔頭
 5. 最後再根據這個檔案的霍夫曼編碼寫入
 6. 在檔案的結尾插入一個數字，表示壓縮碼的最後一個byte的前幾位是有效的(改用bits儲存，最後一個byte可能用到不同bits數量)
 7. 由於要事先寫入霍曼樹，所以當檔案不夠大的時候，檔案有可能會變大
 8. 當檔案中byte的形式越少，壓縮率越大
解壓縮
 1. 一開始根據檔案的檔名以及檔頭寫入的副檔名重新找回檔案名稱
 2. 再根據最後一個byte了解壓縮碼的最後一個byte所使用的bits數量
 3. 根據DFS的方式重新建立起霍夫曼樹
 4. 在根據壓縮的編碼對照霍夫曼樹解碼成不同byte，並寫入重建的檔案
