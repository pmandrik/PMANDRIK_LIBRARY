# PDF COMMON TASKS SOLVERS

попытка убрать отступы от краёв страницы  
`pdfcrop Drevnyaya_Gretsia.pdf`  

убрать отступы вручную
`pdfcrop --margins '0 -40 -140 -40' Drevnyaya_Gretsia-crop.pdf out.pd`

убрать все рисунки, но вместо них пустое пространство  
`gs -o out.pdf  -sDEVICE=pdfwrite -dFILTERIMAGE out2.pdf`  

