import requests
from bs4 import BeautifulSoup
from urllib.parse import urljoin
import json
import re
from datetime import datetime, timedelta
import time
import os


from datetime import datetime


def get_sicau_notices():
    """
    获取四川农业大学生命科学学院通知公告
    """
    # 生命科学学院通知公告页面URL
    base_url = "https://smkx.sicau.edu.cn/xwjtz/tzgg.htm"

    all_notices = []

    try:
        # 设置请求头，模拟浏览器访问
        headers = {
            'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36',
            'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8',
            'Accept-Language': 'zh-CN,zh;q=0.9,en;q=0.8',
            'Accept-Encoding': 'gzip, deflate, br',
            'Connection': 'keep-alive',
            'Referer': 'https://life.sicau.edu.cn/'
        }

        print(f"正在访问四川农业大学生命科学学院通知公告页面...")
        print(f"URL: {base_url}")

        # 发送GET请求
        response = requests.get(base_url, headers=headers, timeout=15)
        response.encoding = 'utf-8'  # 确保中文正确显示

        if response.status_code != 200:
            print(f"请求失败，状态码: {response.status_code}")
            return all_notices

        print("页面访问成功，开始解析内容...")

        # 解析HTML内容
        soup = BeautifulSoup(response.text, 'html.parser')

        # 方法1：尝试查找通知列表容器（基于常见结构）
        notice_list = soup.find('ul', class_=re.compile(r'sub-list|news-list|list-group'))

        # 方法2：如果没找到，尝试查找包含通知标题的所有链接
        if not notice_list:
            print("未找到标准列表容器，尝试其他方法解析...")
            # 查找所有包含日期的元素
            notice_items = soup.find_all(text=re.compile(r'202[0-9]-[0-9]{2}-[0-9]{2}'))
            notice_list = soup  # 在整个页面中搜索

        else:
            # 在列表容器中查找所有通知项
            notice_items = notice_list.find_all('li') or notice_list.find_all('div', class_=re.compile(r'news|notice'))

        # 如果还是没找到，尝试更通用的方法
        if not notice_items:
            # 查找所有包含链接和日期的元素
            all_links = soup.find_all('a', href=True)
            notice_items = []
            for link in all_links:
                if re.search(r'202[0-9]-[0-9]{2}-[0-9]{2}', link.get_text()):
                    notice_items.append(link.parent)

        print(f"找到 {len(notice_items) if notice_items else 0} 个潜在通知项")

        # 提取通知信息
        for i, item in enumerate(notice_items):
            try:
                notice_info = extract_notice_info(item, base_url, i)
                if notice_info:
                    all_notices.append(notice_info)
                    print(f"✓ 已解析: {notice_info['标题']}")

                    # 添加延迟，避免请求过快
                    time.sleep(1)

            except Exception as e:
                print(f"✗ 解析第 {i + 1} 个通知项时出错: {e}")
                continue

        # 如果没有找到通知，尝试直接搜索页面中的文本
        if not all_notices:
            print("尝试直接搜索页面文本内容...")
            all_notices = extract_from_page_text(response.text, base_url)

        return all_notices

    except requests.exceptions.RequestException as e:
        print(f"网络请求错误: {e}")
        return all_notices
    except Exception as e:
        print(f"解析过程中出错: {e}")
        return all_notices


def extract_notice_info(item, base_url, index):
    """
    从单个通知项中提取信息
    """
    try:
        # 查找链接
        link_element = item.find('a', href=True)
        if not link_element:
            return None

        # 提取标题
        title = link_element.get_text(strip=True)
        if not title or len(title) < 5:  # 过滤掉太短的标题
            return None

        # 提取链接
        relative_url = link_element.get('href')
        if not relative_url:
            return None

        # 转换为绝对URL
        full_url = urljoin(base_url, relative_url)

        # 提取日期 - 多种尝试
        date_text = None

        # 方法1：在链接文本中查找日期
        date_match = re.search(r'202[0-9]-[0-9]{2}-[0-9]{2}', link_element.get_text())
        if date_match:
            date_text = date_match.group()

        # 方法2：在父元素中查找日期
        if not date_text:
            parent_text = item.get_text()
            date_match = re.search(r'202[0-9]-[0-9]{2}-[0-9]{2}', parent_text)
            if date_match:
                date_text = date_match.group()

        # 方法3：查找附近的日期元素
        if not date_text:
            date_element = item.find('span', class_=re.compile(r'time|date'))
            if date_element:
                date_match = re.search(r'202[0-9]-[0-9]{2}-[0-9]{2}', date_element.get_text())
                if date_match:
                    date_text = date_match.group()

        # 如果还是没找到日期，设为未知
        if not date_text:
            date_text = "未知日期"

        # 获取通知详情（完整内容）
        detail_content, content_length = get_notice_detail_full(full_url)

        return {
            '序号': index + 1,
            '标题': title,
            '发布时间': date_text,
            '链接': full_url,
            '详情内容': detail_content,  # 完整内容
            '内容长度': content_length,
            '提取时间': datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        }

    except Exception as e:
        print(f"提取通知信息时出错: {e}")
        return None


def get_notice_detail_full(detail_url):
    """
    获取通知详情完整内容（不截断）
    """
    try:
        headers = {
            'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36'
        }

        print(f"  正在获取详情页: {detail_url}")
        response = requests.get(detail_url, headers=headers, timeout=15)
        response.encoding = 'utf-8'

        if response.status_code != 200:
            return f"详情页访问失败: {response.status_code}", 0

        soup = BeautifulSoup(response.text, 'html.parser')

        # 尝试多种内容容器
        content_selectors = [
            'div.content', 'div.article-content', 'div.detail-content',
            'div.news-content', 'div#content', 'div.main-content',
            'article', 'div.entry-content', 'div.v_news_content',
            'div.ny_main', 'div.read', 'div.WordSection1',
            'div.article-body', 'div.article-text'
        ]

        content_element = None
        for selector in content_selectors:
            content_element = soup.select_one(selector)
            if content_element:
                print(f"  使用选择器: {selector}")
                break

        # 如果没找到，尝试查找包含大量文本的div
        if not content_element:
            print("  未找到标准内容容器，尝试智能查找...")
            divs = soup.find_all('div')
            best_div = None
            max_text_length = 0

            for div in divs:
                # 跳过导航、页脚等非内容区域
                if any(cls in str(div.get('class', [])).lower() for cls in
                       ['nav', 'footer', 'header', 'menu', 'sidebar']):
                    continue

                text_length = len(div.get_text(strip=True))
                if text_length > max_text_length and text_length > 100:
                    max_text_length = text_length
                    best_div = div

            if best_div:
                content_element = best_div
                print(f"  找到最长文本div，长度: {max_text_length}字符")

        if content_element:
            # 清理内容，移除不必要的元素
            for element in content_element.find_all(['script', 'style', 'iframe', 'nav', 'header', 'footer', 'aside']):
                element.decompose()

            # 移除空的div和p标签
            for empty_tag in content_element.find_all(['div', 'p']):
                if len(empty_tag.get_text(strip=True)) == 0:
                    empty_tag.decompose()

            # 获取完整的文本内容（不截断）
            text_content = content_element.get_text(strip=False, separator='\n')

            # 清理空白字符但保留段落结构
            text_content = re.sub(r'[ \t]+', ' ', text_content)  # 压缩多个空格
            text_content = re.sub(r'\n[ \t]*\n', '\n\n', text_content)  # 压缩多个空行
            text_content = text_content.strip()

            content_length = len(text_content)
            print(f"  提取内容长度: {content_length}字符")

            return text_content, content_length
        else:
            print("  未找到详细内容")
            return "未找到详细内容", 0

    except Exception as e:
        print(f"  获取详情时出错: {str(e)}")
        return f"获取详情时出错: {str(e)}", 0


def extract_from_page_text(html_content, base_url):
    """
    直接从页面文本中提取通知信息
    """
    notices = []

    try:
        # 使用正则表达式查找通知模式
        pattern = r'([^<>]{10,}?)\s*202[0-9]-[0-9]{2}-[0-9]{2}'
        matches = re.findall(pattern, html_content)

        for i, match in enumerate(matches):
            title = match.strip()
            if len(title) > 10:  # 只保留合理长度的标题
                notices.append({
                    '序号': i + 1,
                    '标题': title,
                    '发布时间': '需进一步解析',
                    '链接': base_url,
                    '详情内容': '需要访问具体页面获取',
                    '内容长度': 0,
                    '提取时间': datetime.now().strftime('%Y-%m-%d %H:%M:%S')
                })

        print(f"从文本中提取到 {len(notices)} 条通知")

    except Exception as e:
        print(f"文本提取时出错: {e}")

    return notices


def filter_notices_by_date(notices, target_date=None):
    """
    按日期筛选通知
    """
    if target_date is None:
        # 默认获取前一天的通知
        target_date = (datetime.now() - timedelta(days=1)).strftime('%Y-%m-%d')

    filtered = [notice for notice in notices if notice['发布时间'] == target_date]
    return filtered


def save_notices_to_file(notices, filename='sicau_notices_full.json'):
    """
    保存通知到JSON文件（包含完整内容）
    """
    try:
        with open(filename, 'w', encoding='utf-8') as f:
            json.dump(notices, f, ensure_ascii=False, indent=2)
        print(f"完整通知已保存到: {filename}")

        # 统计信息
        total_chars = sum(notice.get('内容长度', 0) for notice in notices)
        print(f"总内容字符数: {total_chars}")

    except Exception as e:
        print(f"保存文件时出错: {e}")


def generate_full_summary_file(notices, filename='sicau_notices_full_summary.txt'):
    """
    生成包含完整内容的文本摘要文件
    """
    try:
        with open(filename, 'w', encoding='utf-8') as f:
            f.write("四川农业大学生命科学学院通知公告汇总（完整内容）\n")
            f.write("=" * 60 + "\n")
            f.write(f"生成时间: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
            f.write(f"通知数量: {len(notices)}条\n")

            total_chars = sum(notice.get('内容长度', 0) for notice in notices)
            f.write(f"总内容字符数: {total_chars}\n")
            f.write("=" * 60 + "\n\n")

            for notice in notices:
                f.write(f"【第{notice['序号']}条通知】\n")
                f.write(f"标题: {notice['标题']}\n")
                f.write(f"发布时间: {notice['发布时间']}\n")
                f.write(f"链接: {notice['链接']}\n")
                f.write(f"内容长度: {notice.get('内容长度', 0)}字符\n")
                f.write(f"提取时间: {notice['提取时间']}\n")
                f.write("-" * 50 + "\n")

                # 写入完整内容（不截断）
                f.write("通知内容:\n")
                f.write(notice['详情内容'])
                f.write("\n\n")
                f.write("=" * 80 + "\n\n")

        print(f"完整内容摘要已保存到: {filename}")
        print(f"文件大小: {os.path.getsize(filename)} 字节")

    except Exception as e:
        print(f"生成完整摘要文件时出错: {e}")


def generate_compact_summary(notices, filename='sicau_notices_compact.txt'):
    """
    生成简洁版摘要（只包含标题和基本信息）
    """
    try:
        with open(filename, 'w', encoding='utf-8') as f:
            f.write("四川农业大学生命科学学院通知公告简洁版\n")
            f.write("=" * 50 + "\n")
            f.write(f"生成时间: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
            f.write(f"通知数量: {len(notices)}条\n\n")

            for notice in notices:
                f.write(f"【{notice['序号']}】{notice['标题']}\n")
                f.write(f"   时间: {notice['发布时间']} | 长度: {notice.get('内容长度', 0)}字符\n")
                f.write(f"   链接: {notice['链接']}\n")
                f.write("-" * 40 + "\n")

        print(f"简洁版摘要已保存到: {filename}")

    except Exception as e:
        print(f"生成简洁版摘要时出错: {e}")


def display_notices_preview(notices, title="四川农业大学生命科学学院通知公告"):
    """
    显示通知预览信息（控制台显示时仍然只显示部分内容）
    """
    print(f"\n{'=' * 80}")
    print(f"{title} (共{len(notices)}条)")
    print(f"{'=' * 80}")

    total_chars = sum(notice.get('内容长度', 0) for notice in notices)
    print(f"总内容字符数: {total_chars}")

    for notice in notices:
        print(f"\n【{notice['序号']}】{notice['标题']}")
        print(f"   发布时间: {notice['发布时间']}")
        print(f"   链接: {notice['链接']}")
        print(f"   内容长度: {notice.get('内容长度', 0)}字符")

        # 控制台只显示前200字符预览
        content_preview = notice['详情内容'][:200] + "..." if len(notice['详情内容']) > 200 else notice['详情内容']
        print(f"   内容预览: {content_preview}")
        print("-" * 80)





def send_wechat_push(notices, sckey):
    """
    使用Server酱推送微信通知
    """
    if not notices:
        return False

    # 生成推送内容
    title = f"川农生命学院最新通知({datetime.now().strftime('%m-%d')})"

    # 构建消息内容（限制在2000字符内）
    content = f"📢 四川农业大学生命科学学院\n⏰ 更新时间: {datetime.now().strftime('%Y-%m-%d %H:%M')}\n"
    content += f"📋 共{len(notices)}条新通知\n\n"

    # 只显示前5条通知，避免内容过长
    for i, notice in enumerate(notices[:5]):
        content += f"{i + 1}. {notice['标题']}\n"
        content += f"   时间: {notice['发布时间']}\n"
        if i < len(notices) - 1:  # 不是最后一条就加空行
            content += "\n"

    if len(notices) > 5:
        content += f"\n...还有{len(notices) - 5}条通知，请查看详情"

    content += f"\n\n🔗 查看详情: https://smkx.sicau.edu.cn/xwjtz/tzgg.htm"

    try:
        url = f"https://sctapi.ftqq.com/{sckey}.send"
        data = {
            "title": title,
            "desp": content
        }

        response = requests.post(url, data=data)
        result = response.json()

        if result.get('code') == 0:
            print("✅ 微信推送成功！")
            return True
        else:
            print(f"❌ 微信推送失败: {result.get('message')}")
            return False

    except Exception as e:
        print(f"❌ 微信推送出错: {e}")
        return False


def main():
    """
    主函数
    """
    print("开始爬取四川农业大学生命科学学院通知公告（完整内容版）...")
    print("=" * 60)

    # 获取所有通知
    all_notices = get_sicau_notices()

    if all_notices:
        print(f"\n成功获取 {len(all_notices)} 条通知")

        # 显示预览信息（控制台不显示完整内容）
        display_notices_preview(all_notices)

        # 筛选前一天的通知
        yesterday = (datetime.now() - timedelta(days=1)).strftime('%Y-%m-%d')
        yesterday_notices = filter_notices_by_date(all_notices, yesterday)

        if yesterday_notices:
            print(f"\n昨天({yesterday})发布的新通知:")
            display_notices_preview(yesterday_notices, f"前一天({yesterday})通知")
        else:
            print(f"\n昨天({yesterday})没有发布新通知")

        # 保存完整内容到JSON文件
        save_notices_to_file(all_notices)

        # 生成包含完整内容的文本文件
        generate_full_summary_file(all_notices)

        # 生成简洁版摘要
        generate_compact_summary(all_notices)

        print("\n所有文件已生成完成！")
        print("1. sicau_notices_full.json - 完整JSON数据")
        print("2. sicau_notices_full_summary.txt - 完整内容文本版")
        print("3. sicau_notices_compact.txt - 简洁版摘要")

    else:
        print("未能获取到任何通知信息")
    if all_notices:
        # Server酱推送（只需要一个KEY）
        sckey = "SCT301452TyhqmvrehAIcI5GoDT6yKqxTB"  # 在 https://sct.ftqq.com 免费获取
        send_wechat_push(all_notices, sckey)


if __name__ == "__main__":
    main()