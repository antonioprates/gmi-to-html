// gmi-to-html utility, using Sugar-C in about 150 lines of code
// 2021, by Antonio Prates <antonioprates at gmail dot com>

#include <string.h>
#include <sugar.h>

string title;
bool preformattedMode;
number linkCount;

stringList preProcess(string text) {
  text = replaceWord(text, "\r", "");       // remove \r
  text = replaceWord(text, "&", "&amp;");   // html escape codes
  text = replaceWord(text, "<", "&lt;");    // html escape codes
  text = replaceWord(text, ">", "&gt;");    // html escape codes
  text = replaceWord(text, "```\n", "```"); // hack/fix extra space :P
  text = replaceWord(text, "\n", "\n<br />\n");
  return splitSep(text, '\n'); // ...and split
}

string postProcess(number lineCount, stringList lines) {
  string head = // join/add title in head
      join3s("<!DOCTYPE HTML>\n<html>\n<head>\n<meta charset='utf-8'>\n"
             "<link rel='stylesheet' href='styles.css' type='text/css' />\n"
             "<link rel='icon' href='favicon.ico' type='image/x-icon' />\n",
             title, "</head>\n<body>\n<div id='root'>\n<div id='content'>\n");
  string body = joinSep(lineCount, lines, '\n'); // join lines alltogether..
  // hacks and fixes for spacing issues, might create their own issues :P
  body = replaceWord(body, "\n\n", "\n");
  body = replaceWord(body, "</h1>\n<br />\n<br />", "</h1>");
  body = replaceWord(body, "</h1>\n<br />", "</h1>");
  body = replaceWord(body, "</h2>\n<br />\n<br />", "</h2>");
  body = replaceWord(body, "</h2>\n<br />", "</h2>");
  body = replaceWord(body, "</h3>\n<br />\n<br />", "</h3>");
  body = replaceWord(body, "</h3>\n<br />", "</h3>");
  body = replaceWord(body, "</li>\n<br />", "</li>");
  body = replaceWord(body, "</pre>\n<br />", "</pre>");
  body = replaceWord(body, "</blockquote>\n<br />", "</blockquote>");
  body = replaceWord(body, "<hr />\n<br />", "<hr />");
  // ...end of spacing hacks and fixes
  string ending = "\n<br />\n<br />\n</div>\n</div>\n</body>\n</html>\n";
  return replaceWord(join3s(head, body, ending), "\n", "\r\n"); // add \r :)
}

string toLink(string line) {
  stringList words = splitSep(line, ' '); // tokenize the link
  number wordCount = listCount(words);    // count our tokens
  if (wordCount < 2)                      // fail-safe for missing link
    return "- missing link -";
  string url = words[1];  // get url
  bool isInternal = true; // adapt internal/external links:
  string externalRef[6] = {"http://",   "https://",  "ftp://",
                           "gemini://", "gopher://", "www."};
  for (number i = 0; i < 6; i++)
    if (startsWith(url, externalRef[i]))
      isInternal = false;
  url = isInternal ? replaceWord(url, ".gmi", ".html") : url;
  string description = // join the rest as description OR use url as desc.
      wordCount > 2 ? joinSep(listCount(&words[2]), &words[2], ' ') : url;
  string linkNumber = join3s("[", ofNumber(++linkCount), "] <a href='");
  return join5s(linkNumber, url, isInternal ? "'>" : "' target='_blank'>",
                description, "</a>");
}

string toHTML(string line) {
  // **preformatted mode**
  if (startsWith(line, "```")) {          // ``` -> preformatted text
    preformattedMode = !preformattedMode; // toggle global <pre> mode
    line = preformattedMode ? replaceWord(line, "```", "<pre>\n")
                            : replaceWord(line, "```", "</pre>");
  }
  if (preformattedMode)                     // while global <pre> mode
    return replaceWord(line, "<br />", ""); // -> revert preProcess
  // **simple blockquote**
  if (startsWith(line, "&gt; ")) {          // > -> blockquote (escaped)
    line = replaceWord(line, "<br />", ""); // -> revert preProcess
    return join3s("<blockquote>", &line[5], "</blockquote>\n");
  }
  // **links**
  if (startsWith(line, "=&gt;")) // => link (escaped)
    return toLink(line);         // -> <a href...
  // **headings**
  if (startsWith(line, "### ")) // ### -> h3
    return join3s("<h3>", &line[4], "</h3>");
  if (startsWith(line, "## ")) // ## -> h2
    return join3s("<h2>", &line[3], "</h2>");
  if (startsWith(line, "# ")) { // # -> h1 ...AND set global page title!
    title = join3s("<title>", &line[2], "</title>\n");
    return join3s("<h1>", &line[2], "</h1>");
  }
  // **horizontal-rule**
  if (areSame(line, "---"))
    return "<hr />";
  // **inline code**
  number length;
  if (countWord(line, "`") > 1) {               // simple check, no guarantee
    line = replaceWord(line, " `", " <code>");  // start inline code
    line = replaceWord(line, "` ", "</code> "); // end inline code
    if (startsWith(line, "`"))                  // if first thin in the line
      line = join2s("<code>", &line[1]);        // begin with inline code
    length = strlen(line);
    if (length > 0 && line[length - 1] == '`')  // if last...
      line = replaceWord(line, "`", "</code>"); // end inline code
  }
  // **inline bold**
  if (countWord(line, "*") > 3) {             // simple check, no guarantee
    line = replaceWord(line, " **", " <b>");  // start inline bold text
    line = replaceWord(line, "** ", "</b> "); // end inline bold text
    if (startsWith(line, "**"))               // if first thin in the line
      line = join2s("<b>", &line[2]);         // being with inline code
    length = strlen(line);                    // ... if last...
    if (length > 1 && line[length - 1] == '*' && line[length - 2] == '*')
      line = replaceWord(line, "**", "</b>"); // end bold text
  }
  // **list items**
  if (startsWith(line, "* "))                 // * -> list item
    return join3s("<li>", &line[2], "</li>"); // end inline bold text
  // ...or else -> **just simple text**
  return line;
}

void convert(string path) {
  if (countWord(path, ".gmi")) {    // ultra-simple path validadion
    string text = readFile(path);   // read and hope content is text/gemini :D
    if (text) {                     // safe-gard / read failure
      title = "<title>#</title>\n"; // reset global 'page title' for each file
      preformattedMode = false;     // reset global '<pre> mode' for each file
      linkCount = 0;                // reset global 'link count' for each file
      stringList lines = preProcess(text);
      number lineCount = listCount(lines);
      for (number i = 0; i < lineCount; i++)
        lines[i] = toHTML(lines[i]); // do the magic :D
      string newPath = replaceWord(path, ".gmi", ".html");
      if (writeFile(postProcess(lineCount, lines), newPath)) // and save...
        return println(join5s("'", path, "' -> '", newPath, "'"));
    }
    println(join3s("'", path, "' -> failed!")); // ...or complain!
  }
}

app({
  if (argc < 2)
    println("please, provide '.gmi' file paths as arguments...");
  else
    forEach(argc - 1, &argv[1], &convert);
})
