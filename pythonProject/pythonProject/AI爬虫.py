import requests
from bs4 import BeautifulSoup
import json
from urllib.parse import urljoin  # 用于拼接完整的URL

def crawl_notices(list_url):
    """
    从四川农业大学生命科学学院通知公告列表页抓取所有通知的详细信息
    """
    all_notices = []  # 用于存储所有通知信息

    # 1. 抓取列表页
    print(f"正在抓取列表页: {list_url}")
    try:
        response = requests.get(list_url)
        response.encoding = 'utf-8'  # 设置编码，防止中文乱码
        response.raise_for_status()  # 检查请求是否成功
    except requests.exceptions.RequestException as e:
        print(f"抓取列表页失败: {e}")
        return all_notices

    # 2. 解析列表页
    soup = BeautifulSoup(response.text, 'html.parser')

    # 3. 查找所有通知条目
    # 需要根据实际网页结构来定位，这里是一个示例选择器
    # 通常通知会放在一个列表（<ul>）或表格（<table>）中，每个<li>或<tr>是一条通知
    notice_items = soup.select('ul.list-group li a')  # 请根据实际HTML结构调整此选择器

    # 示例：如果通知链接是这种结构 <a href="/info/1025/18288.htm">【评优公示】关于...</a>
    # 那么选择器可能是 'a' 并过滤出包含特定关键词的链接

    for index, item in enumerate(notice_items):
        # 提取通知标题
        title = item.get_text().strip()
        # 提取详情页链接（可能是相对路径）
        relative_url = item.get('href')

        # 跳过无效链接
        if not relative_url or not title:
            continue

        # 将相对URL转换为绝对URL
        detail_url = urljoin(list_url, relative_url)

        print(f"({index+1}/{len(notice_items)}) 正在抓取详情页: {title}")

        # 4. 抓取并解析详情页
        notice_detail = crawl_detail_page(detail_url, title)
        if notice_detail:
            all_notices.append(notice_detail)

    return all_notices

def crawl_detail_page(detail_url, title):
    """
    抓取单个通知详情页的内容
    """
    try:
        response = requests.get(detail_url)
        response.encoding = 'utf-8'
        response.raise_for_status()
    except requests.exceptions.RequestException as e:
        print(f"  抓取详情页失败: {e}")
        return None

    soup = BeautifulSoup(response.text, 'html.parser')

    # 5. 提取详情页中的信息
    # 需要根据实际网页结构调整选择器

    # 提取发布日期
    # 示例：假设发布日期在 <div class="info"> 下的某个位置
    date_element = soup.find('div', class_='info')  # 请根据实际HTML结构调整
    publish_date = date_element.get_text().strip() if date_element else "未知日期"

    # 提取正文内容
    # 示例：假设正文在一个id为`content`的div里
    content_element = soup.find('div', id='content')  # 请根据实际HTML结构调整
    if content_element:
        # 清理内容，去除不必要的标签和空白
        content = content_element.get_text(strip=True, separator='\n')
    else:
        content = "正文内容获取失败"

    # 返回结构化数据
    notice_info = {
        "title": title,
        "publish_date": publish_date,
        "detail_url": detail_url,
        "content": content
    }

    return notice_info

def save_to_json(notices, filename='notices.json'):
    """将通知数据保存为JSON文件"""
    with open(filename, 'w', encoding='utf-8') as f:
        json.dump(notices, f, ensure_ascii=False, indent=2)
    print(f"数据已保存到 {filename}")

# 主程序
if __name__ == "__main__":
    # 四川农业大学生命科学学院通知公告列表页URL
    # 注意：这个URL需要是您提供的那个页面的实际地址，而不是文本内容
    list_url = "https://life.sicau.edu.cn/xxx/xxx.htm"  # 请替换为真实的URL

    notices_data = crawl_notices(list_url)

    if notices_data:
        print(f"\n成功抓取 {len(notices_data)} 条通知！")
        # 打印第一条通知的内容作为示例
        if notices_data:
            print("\n=== 第一条通知详情 ===")
            print(json.dumps(notices_data[0], ensure_ascii=False, indent=2))
        # 保存所有数据
        save_to_json(notices_data)
    else:
        print("未能抓取到任何通知数据。")