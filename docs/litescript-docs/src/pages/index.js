import clsx from 'clsx';
import Link from '@docusaurus/Link';
import useDocusaurusContext from '@docusaurus/useDocusaurusContext';
import Layout from '@theme/Layout';
import HomepageFeatures from '@site/src/components/HomepageFeatures';

import Heading from '@theme/Heading';
import styles from './index.module.css';

function HomepageHeader() {
  const {siteConfig} = useDocusaurusContext();
  return (
    <header className={clsx('hero hero--primary', styles.heroBanner)}>
      <div className="container">
        <Heading as="h1" className="hero__title">
          {`LiteScript`}
        </Heading>
        <p className="hero__subtitle">{`A powerful toy programming language.`}</p>
      </div>
    </header>
  );
}

export default function Home() {
  const {siteConfig} = useDocusaurusContext();
  return (
    <Layout
      title={`Introduction`}
      description="">
      <HomepageHeader />
      <main>
        <div id="frame">
          <img id="mainlogo" src="/img/logo.svg"/>
        </div>
        <div id="desc">
        <p>
          LiteScript is a dynamic procedural fixed-form line based interpreted toy programming 
          language initially built just for turing completeness that has expanded to provide a 
          wide variety of functionalities suitable for executing small and simple scripts and 
          prototypes.
        </p>
        <p>
          Its syntax is based off a high level dynamic scripting language like Python while
          adhering to the philosophy and design principals of lower level languages like assembly
          and the C programming language, to create a scripting paradigm that sticks to a 
          style closely resembling to how a computer actually runs under the hood.
        </p>
        <p>
          LiteScript also comes with integrated modules whose capabilities and functionalities
          include but are not limited to:
          <ol>
            <li>Concurrent and parallel execution via multithreading.</li>
            <li>Systems programming like process control and heap management.</li>
            <li>Filesystem manipulation and interaction.</li>
            <li>A networking library able to perform and handle simple HTTP requests and responses.</li>
            <li>A very simple GUI library to create simple windows and forms.</li>
            <li>Database connections and interactions with transactions via SQLite and PostgreSQL.</li>
          </ol>
          Combining the functionality of the different integrated modules we extend LiteScript 
          from just turing completeness to allow it to even set up simple server-side applications.
        </p>
        <p>
          In the following reference we try to reference all the relevant core functionalities and 
          utilities that the language provides, so that you, the programmer, can utilize them to 
          their fullest extent in your scripts.
        </p>

        <h1>Philosophy</h1>
        <p>
          Since LiteScript is an open source project, we want to make the language and its runtime
          accessible to everybody, taking a firm stance against corporate proprietary software,
          therefore, the interpreter/runtime is designed to run <strong>exclusively on the Linux kernel</strong>,
          allowing LiteScript to be executed in any Linux distribution (Ubuntu, Arch, Kali, Mint, etc.).
        </p>
        <p>
          Despite LiteScript's primary purpose of being a dynamic scripting language it borrows the
          philosophy and design principals of assembly, therefore its style reflects closely to how a
          computer executes programs, while only providing the abstractions we deem necessary for dynamic
          scripting. By adhering to this philosophy, LiteScript will only support imperative
          and procedural programming, since at a low level this is what gets compiled/assembled to, and
          will <strong>never support Object-Oriented Programming</strong>. Everything that requires the program to handle
          mutable states and custom objects will be handled implicitly by the interpreter's runtime.
        </p> 
        <h1>Interpreter</h1>
        <p>
          LiteScript is executed with its own interpreter, called 'Lite'. To install it, just follow
          the installation steps found in the <a href="https://github.com/gderrr/LiteScript">LiteScript repository</a>.
        </p>
        <p>
          Once installed correctly into the binaries of your Linux machine, just run it with the following command:
        </p>
        <pre>
            <code>lite [FILENAME] [ARGS...]</code>
        </pre>
        <p>
          Where the FILENAME is a file ending with the .lts extension containing your LiteScript program,
          and ARGS... are any additional arguments you want to run with your program, just like a C or C++ program.
        </p>
        <p>
          You can also check the version and release date of your LiteScript interpreter by running lite with 
          the following flag:
        </p>
        <pre>
          <code>lite --version</code>
        </pre>

        </div>
      </main>
    </Layout>
  );
}
