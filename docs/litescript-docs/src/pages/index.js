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
          {siteConfig.title}
        </Heading>
        <p className="hero__subtitle">{siteConfig.tagline}</p>
      </div>
    </header>
  );
}

export default function Home() {
  const {siteConfig} = useDocusaurusContext();
  return (
    <Layout
      title={`Hello from ${siteConfig.title}`}
      description="Description will go into a meta tag in <head />">
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
          adhering to the philosophy and design principals of lower level language like assembly
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
          Combining the functionality of the different integrated modules we extend LiteScript from
          just turing completeness to allow it to even set up simple server-side applications.
        </p>
        </div>
      </main>
    </Layout>
  );
}
