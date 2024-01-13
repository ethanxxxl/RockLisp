;;; Programmer: Ethan Smith
;;; Date: 12 January 2024
;;; Purpose: this is a syntax checking file for my rocklisp compiler.
;;;          it essentially unit tests the compiler to ensure that
;;;          valid code compiles.

[ ]
[token1[next subsexp]]
[even more condensed[sexp]interspersed[with[even]]more[[[sexps]]]]
[ hello this is a test ]
[ another test ]
[ how many can I do? ]
[ this should be fine! ]
 
 
[ [ what about a double list ] ]
[ [ what about a double] [ double list ] ]
[ [ sexp ] interspersed [ with [ even ] ] more [ [ [ sexps ] ] ] ]
 
[what if it is condensed?]
[ conddensed [ssexp] interspsersed [wdith [even]] more [[[sexps]]] ]

[ what about {with weird syntax }]
[ this "is a test" " for " "what happens" with "parenthesis"]

[asd asdf"hjkl hjkl"]
["asdf asdf"["asd asd hjkl"]]

[numbers-test: 1 2 skip a few 012 0x99 01000 232]
;;; [invalid nums: 235dsa 023xda 39z2dfs]

[ what happens "here"]

[string-test "abc"[xyz] "abc""def" 123"abc"123 abc["def"]"abc"]

[ braces {arent supported yet}]
[ braces { arent supported yest }]
[ braces really{arent supported}yet]

; what happens with an empty list?

[more testing]
[]
[ ]

[
]

[     ; extra spaces.
]

[     ; extra newlines


]

[nil [] within [] []  [] this list]
[nil [ sdf [ ] [   ] [] ] [] ]

[ nil [ within
        [  ]
        [  ]
        this
        list]]

;; multi line sexps
[ this
  is
  a multiline [s-expression that
                            has
               multi-levels]
               ]

;; practical example
[fn main nil
    [return 0]]

[fn main[nil]
    [return 0]]