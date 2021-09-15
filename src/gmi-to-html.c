// gmi-to-html utility, using Sugar-C in about 150 lines of code
// 2021, by Antonio Prates <antonioprates at gmail dot com>

#include <string.h>
#include <sugar.h>

string title;
bool preformattedMode;
bool quoteMode;
number linkCount;

stringList preProcess(string text) {
  text = replaceWord(text, "\r", "");       // remove \r
  text = replaceWord(text, "\"", "&quot;"); // html escape codes
  text = replaceWord(text, "&", "&amp;");   // html escape codes
  text = replaceWord(text, "<", "&lt;");    // html escape codes
  text = replaceWord(text, ">", "&gt;");    // html escape codes
  text = replaceWord(text, "\n##", "##");   // hack/fix extra space :P
  text = replaceWord(text, "```\n", "```"); // hack/fix extra space :P
  text = replaceWord(text, "\n\n", "\n<br />\n<br />\n"); // break lines
  return splitSep(text, '\n');                            // ...and split
}

string postProcess(number lineCount, stringList lines) {
  string head = // join/add title in head
      join3s("<!DOCTYPE HTML>\n<html>\n<head>\n<meta charset='utf-8'>\n"
             "<link rel='stylesheet' href='styles.css' type='text/css' />\n"
             "<link rel='icon' href='favicon.ico' type='image/x-icon' />\n",
             title, "</head>\n<body>\n<div id='root'>\n<div id='content'>\n");
  string body = joinSep(lineCount, lines, '\n'); // join lines alltogether..
  body = replaceWord(body, "</h1>\n<br />\n<br />", "</h1>"); // hack/fix :P
  body = replaceWord(body, "</h2>\n<br />\n<br />", "</h2>"); // hack/fix :P
  body = replaceWord(body, "</h3>\n<br />\n<br />", "</h3>"); // hack/fix :P
  body = replaceWord(body, "</li>\n<br />", "</li>");         // hack/fix :P
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
  // **block modes**
  if (!quoteMode) {
    if (startsWith(line, "```")) {          // ``` -> preformatted text
      preformattedMode = !preformattedMode; // toggle global <pre> mode
      line = preformattedMode ? replaceWord(line, "```", "<pre>\n")
                              : replaceWord(line, "```", "</pre>");
    }
    if (preformattedMode)            // while global <pre> mode
      return line;                   // -> pure simple text
    if (startsWith(line, "&gt; ")) { // > -> blockquote (escaped)
      quoteMode = true;              // global quote mode on
      line = replaceWord(line, "<br />", "");
      return join2s(replaceWord(line, "&gt; ", "<blockquote>\n"), "<br />");
    }
  }
  if (quoteMode) { // the previous !quoteMode logic, sucks... I know
    line = replaceWord(line, "<br />", "");
    if (startsWith(line, "&gt; ")) // while global quote mode
      return join2s(replaceWord(line, "&gt; ", ""), "<br />");
    else {
      quoteMode = false;                      // global quote mode off
      line = join2s("</blockquote>\n", line); // end blockquote
      line = replaceWord(line, "\n\n", "\n"); // hack/fix extra space :P
    }
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
  // **inline formatting**
  line = replaceWord(line, " `", " <code>"); // start inline code
  if (startsWith(line, "`"))
    line = replaceWord(line, "`", "<code>");  // start inline code
  line = replaceWord(line, "` ", "</code> "); // end inline code
  number length = strlen(line);
  if (length > 0 && line[length - 1] == '`')
    line = replaceWord(line, "`", "</code>"); // end inline code
  line = replaceWord(line, " **", " <b>");    // start bold text
  if (startsWith(line, "**"))
    line = replaceWord(line, "**", "<b>");  // start bold text
  line = replaceWord(line, "** ", "</b> "); // end bold text
  length = strlen(line);
  if (length > 1 && line[length - 1] == '*' && line[length - 2] == '*')
    line = replaceWord(line, "**", "</b>"); // end bold text
  if (startsWith(line, "* "))               // * -> list item
    return join3s("<li>", &line[2], "</li>");
  // ...or else -> **just simple text**
  return line;
}

void convert(string path) {
  if (countWord(path, ".gmi")) {   // simple path validadion
    string text = readFile(path);  // read and hope content is a text file
    if (text) {                    // safe-gard / read failure
      title = "untitled document"; // reset global 'page title' for each file
      preformattedMode = false;    // reset global '<pre> mode' for each file
      quoteMode = false;           // reset global 'quote mode' for each file
      linkCount = 0;               // reset global 'link count' for each file
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
