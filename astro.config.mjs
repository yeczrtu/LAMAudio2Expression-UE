import { defineConfig } from 'astro/config';
import starlight from '@astrojs/starlight';

export default defineConfig({
  site: 'https://yeczrtu.github.io',
  base: '/LAMAudio2Expression-UE',
  trailingSlash: 'always',
  integrations: [starlight({
    title: 'LAM Audio2Expression',
    description: 'Unreal Engineで音声からARKit 52表情カーブを生成するプラグインのドキュメント。',
    defaultLocale: 'root',
    locales: {
      root: { label: '日本語', lang: 'ja' },
      en: { label: 'English', lang: 'en' },
    },
    favicon: '/favicon.svg',
    social: [{ icon: 'github', label: 'GitHub', href: 'https://github.com/yeczrtu/LAMAudio2Expression-UE' }],
    editLink: { baseUrl: 'https://github.com/yeczrtu/LAMAudio2Expression-UE/edit/codex/docs-site/' },
    lastUpdated: false,
    disable404Route: true,
    customCss: ['./src/styles/custom.css'],
    expressiveCode: {
      // Keep code controls localized on Windows paths and project-base URLs alike.
      getBlockLocale: ({ file }) => /(?:^|\/)en\//.test(
        `${file.path ?? ''} ${file.url?.pathname ?? ''}`.replaceAll('\\', '/')
      ) ? 'en' : 'ja',
    },
    components: {
      Head: './src/components/Head.astro',
      PageTitle: './src/components/PageTitle.astro',
      Footer: './src/components/Footer.astro',
    },
    sidebar: [
      { label: 'はじめる', translations: { en: 'Get started' }, items: [
        { slug: '' }, { slug: 'lip-sync-comparison' }, { slug: 'installation' }, { slug: 'demo' },
      ] },
      { label: '使い方', translations: { en: 'Guides' }, items: [
        { slug: 'blueprint' }, { slug: 'expression-curves' }, { slug: 'playback' },
        { slug: 'live-input' }, { slug: 'models-and-packaging' },
      ] },
      { label: '開発・リファレンス', translations: { en: 'Reference' }, items: [
        { slug: 'development' }, { slug: 'architecture' }, { slug: 'validation' },
        { slug: 'troubleshooting' }, { slug: 'licenses' },
      ] },
    ],
  })],
});
