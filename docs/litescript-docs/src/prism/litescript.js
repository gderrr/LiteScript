import Prism from 'prismjs';

console.log('Loaded custom Prism language: litescript');

Prism.languages.litescript = {
  comment: /#.*/,
  keyword: /\b(fn|let|return|if|else)\b/,
  number: /\b\d+\b/,
  string: /"(?:\\.|[^"\\])*"/,
  operator: /[=+\-*\/]/,
  punctuation: /[{}()[\];,]/,
};

export default Prism.languages.litescript;