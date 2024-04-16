;;; Directory Local Variables            -*- no-byte-compile: t -*-
;;; For more information see (info "(emacs) Directory Variables")

((nil    . ((projectile-project-compilation-cmd . "make")
            (projectile-project-configure-cmd   . "make clean")
            (+license/license-choice            . "GPLv2")))
 (c++-mode . ((flycheck-gcc-include-path          . (".." "../avm"))
              (flycheck-clang-include-path        . (".." "../avm"))
              (company-clang-arguments            . ("-I.." "-I../avm"))
              (eval                               . (clang-format-mode t)))))
