import requests

def download_manga_images(image_urls):
    headers = {
        'Referer': 'https://zonatmo.com/',
        'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36'
    }
    
    for i, url in enumerate(image_urls, 1):
        try:
            response = requests.get(url, headers=headers)
            if response.status_code == 200:
                with open(f'pagina_{i}.webp', 'wb') as f:
                    f.write(response.content)
                print(f'✓ Imagen {i} descargada: {url}')
            else:
                print(f'✗ Error {response.status_code} en imagen {i}')
        except Exception as e:
            print(f'✗ Error descargando imagen {i}: {e}')

image_urls = [
    'https://img1tmo.com/uploads/20210626/87f93c3f1ac206c9b360d98b078a4406/ffd339ba.webp',
]

download_manga_images(image_urls)
