import React from 'react';
import { Highlight, Prism, themes } from 'prism-react-renderer';

Prism.languages.litescript = {
  'comment': {
    pattern: /\$.*$/,
    greedy: true
  },
  'keyword': {
    pattern: /\b(opt|conditional|loop|return|exit|start|import|require)\b/,
    greedy: true
  },
  'number': {
    pattern: /\b\d+(?:\.\d+)?\b/,
    greedy: true
  },
  'operator': {
    pattern: /(\|\||&&|\||\^|&|=|\+=|-=|==|!=|<=|<|>=|>|<<|>>|\+|-|\*\*|\*|\/|%|!|~)/,
    greedy: true
  },
  'string': {
    pattern: /"(?:(?:\\.)|[^"\\])*"/,
    greedy: true,
    inside: {
      'escape': {
        pattern: /\\[nrt0"\\]/,
        alias: 'constant'
      },
      'invalid': {
        pattern: /\\./,
        alias: 'error'
      },
      'punctuation': {
        pattern: /^"|"$/,
        alias: 'punctuation'
      }
    }
  },
  'class-name': {
    pattern: /\b(container|global)\b/,
    greedy: true
  },
  'function': [
    {
      pattern: /\b(get|put|del|top|end|psh|pop|deq|num|igtk|igtv)\b/,
      alias: 'builtin'
    },
    {
      pattern: /\b\w+:/,
      alias: 'symbol'
    },
    {
      pattern: /^[A-Za-z_]\w*/m,
      alias: 'declaration'
    }
  ],
  'symbol': {
    pattern: /\b\w+;/,
    alias: 'class-name'
  },
  'variable': {
    pattern: /\b[A-Za-z_]\w*\b/
  }
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
  const precode = flattenChildren(children);
  const code = precode.replace(/\n[ \t]*$/, '');

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