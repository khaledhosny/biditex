BiDiTeX
=======

BiDiTeX is a pre-processor for LaTeX files to support bidirectional text.

Usage
-----

To use BiDiTeX, run the `biditex` command on the LaTeX file, the resulting file
can then be processed by the TeX engine to produce the final output.

BiDiTeX understands some special comments to control its behaviour:
* `%BIDION`: activate BiDiTeX special handling.
* `%BIDIOFF`: stop it
* `%BIDILTR`: the document is mainly left-to-right with some right-to-left text.

Example
-------

Here a sample XeLaTeX document:

    \documentclass{article}
    
    \usepackage{polyglossia}
    \setmainlanguage[locale=mashriq]{arabic}
    \setotherlanguage{english}
    
    \setmainfont[Script=Arabic]{Amiri}
    \newfontfamily\englishfont[Script=Latin]{Amiri}
    
    \let\L=\textenglish % BiDiTeX uses \L{} for left-to-right text
    \let\R=\textarabic  % while \R{} is used to right-to-left text
    
    %BIDION
    \begin{document}
    \section{رخصة جنو العمومية}
    
    \emph{رخصة جنو العمومية} (بالإنجليزية: GNU General Public License؛ تختصر GPL)
    رخصة برمجيات حرة مستخدمة على نحو واسع، كتبها أصلا ريتشارد ستولمن لمشروع جنو.
    رخصة جنو العمومية هي أشهر مثال معروف للحقوق المتروكة المتشددة التي تطالب أن
    ترخّص الأعمال المشتقة تحت نفس الرخصة، وبناءً على هذه الفلسفة، يقال أن رخصة جنو
    العمومية تزيد عائدات البرنامج الحاسوبي من تعريف البرمجيات الحرة، وتستخدم الحقوق
    المتروكة لضمان الحرية الفعلية، حتى عندما يُغيّر العمل أو يضاف إلى آخر. هذا يختلف
    عن رخص البرمجيات الحرة المتساهلة التي تمثل رخص بي إس دي مثالا قياسيا لها.
    
    \end{document}
    %BIDIOFF

This can be processed with BiDiTeX, then by XeTeX to produce final PDF:

    biditex -n test.tex -o test-bidi.tex
    xelatex test-bidi.tex

