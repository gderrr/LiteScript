import React from 'react';
import { Highlight, Prism, themes } from 'prism-react-renderer';

Prism.languages.litescript = {
    comment: /\$.*$/m,
    keyword: /\b(opt|conditional|loop|return|exit|start|import|require|container|global)\b/,
    number: /\b\d+(?:\.\d+)?\b/,
    operator: /\|\||&&|\||\^|&|=|\+=|-=|==|!=|<=|<|>=|>|<<|>>|\+|-|\*\*|\*|\/|%|!|~/,
    string: {
        pattern: /"(?:\\[nrt0"\\]|[^"\\])*"/,
        greedy: true,
    },
    function: /\b(get|put|del|top|end|psh|pop|deq|num|igtk|igtv)\b/,
    'function-call': /\b\w+:/,
    'function-declaration': {
        pattern: /^(?!\t).+$/m,
        alias: 'function',
    },
    className: /\b\w+;/,
    variable: /\b[A-Za-z_][\w]*\b/,
};

export default function CodeBlockLTS({ children }) {
  const theme = themes.vsDark;

  // Flatten MDX children to plain string
  const flattenChildren = (children) => {
    let result = '';
    React.Children.forEach(children, (child) => {
      if (typeof child === 'string') result += child;
      else if (React.isValidElement(child) && child.props.children) {
        result += flattenChildren(child.props.children);
      }
    });
    return result;
  };
  const code = flattenChildren(children).trim();

  return (
    <Highlight
      Prism={Prism}
      code={code}
      language="litescript"
      theme={theme}
    >
      {({ className, style, tokens, getLineProps, getTokenProps }) => (
        <pre className={className} style={{ ...style, padding: '1rem', overflowX: 'auto' }}>
          {tokens.map((line, i) => (
            <div key={i} {...getLineProps({ line, key: i })}>
              {line.map((token, key) => (
                <span key={key} {...getTokenProps({ token, key })} />
              ))}
            </div>
          ))}
        </pre>
      )}
    </Highlight>
  );
}