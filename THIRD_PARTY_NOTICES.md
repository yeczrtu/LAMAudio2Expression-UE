# Documentation site attribution

Documentation is edited and translated from the public LAM Audio2Expression plugin
and demo documents at the commits in `content-sources.json`. Keep the source links
attached to each page and the upstream MIT notice in `LICENSE`.

The site uses Astro and Starlight (MIT) and their dependencies. Package versions and
upstream license information are recorded in `package-lock.json` and the packages.

## Demo media

`public/images/demo-preview.png` and `public/images/face-demo.png` are unchanged
copies from the pinned demo repository. The original video attachment is unavailable
(HTTP 404 checked 2026-09-28); the video is not redistributed in this branch. The upstream preview was
extracted at five seconds. Audio and the audio-synchronized demo presentation are
CC BY-SA 4.0: https://creativecommons.org/licenses/by-sa/4.0/

Speech: **JVNV: A Corpus of Japanese Emotional Speech with Verbal Content and
Nonverbal Expressions** by **Detai Xin, Junfeng Jiang, Shinnosuke Takamichi,
Yuki Saito, Akiko Aizawa, Hiroshi Saruwatari**.
https://sites.google.com/site/shinnosuketakamichi/research-topics/jvnv_corpus

Adaptation: **litagin**, `jvnv_corpus_v1_no_nv`, revision
`0ca4908ee5b9610bc3b739e1e67dff6120df0675`, removing nonverbal intervals.
https://huggingface.co/datasets/litagin/jvnv_corpus_v1_no_nv

Character: **hinzka / 52blendshapes-for-VRoid-face**, based on VRoid Studio / pixiv.
The character is not relicensed under MIT or CC0. Author permission and the older
VRM metadata discrepancy are preserved in the original demo attribution:
https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Resources/Demo/README.md

Changes in the upstream demo include FBX migration, extracted textures, UE lit
materials, SoundWave import, Bink encoding, and generated facial animation.
Attribution does not imply endorsement. The site's Japanese and English license
pages preserve the original creators, terms, links, and modification notices.

The neural model and upstream LAM code referenced by the guides remain Apache-2.0;
they are not shipped in this documentation branch.
