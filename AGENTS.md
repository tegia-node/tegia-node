# AGENTS Rules For `tegia-node`

## Documentation Split

- Use `/docs` only for internal runtime architecture and implementation details of `tegia-node`.
- Use `/include/tegia/docs` for SDK documentation intended for actor configuration repositories (`*-conf`) and other users of public Tegia headers.
- When writing docs for configuration authors, place or update them in `/include/tegia/docs` first.

## Language Policy

- All project documentation must be written in Russian.
- All code comments must be written in Russian.

## Правила сборки

- Проект собирается через CMake-артефакты в каталоге `/build`.
- Для проверки изменений запускай `make` из каталога `/build`.
- Не используй `npm run build` для сборки `tegia-node`: в каталоге `/build` нет `package.json`, а сборка проекта не завязана на npm.
