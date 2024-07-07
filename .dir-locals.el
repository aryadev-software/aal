;;; Directory Local Variables            -*- no-byte-compile: t -*-
;;; For more information see (info "(emacs) Directory Variables")

((nil      . ((+license/license-choice     . "GNU General Public License Version 2")
              (compile-command             . "make all VERBOSE=2 RELEASE=0")))
 (c++-mode . ((flycheck-gcc-include-path   . (".." "../avm"))
              (flycheck-clang-include-path . (".." "../avm"))
              (company-clang-arguments     . ("-I.." "-I../avm"))
              (mode                        . clang-format)
              (eval                        . (eglot-ensure)))))
