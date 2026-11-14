/***************************************************************
                          page_rom.cpp

Handle the individual ROM page.
***************************************************************/

#include <string.h>
#include <ctime>
#include <sstream>
#include <fstream>
#include <iomanip>
#include "category.h"
#include "page_rom.h"
#include "page_author.h"
#include "markdown.h"
#include "helper.h"


/*==============================
    Compile
    Compiles an individual ROM page
==============================*/

void Page_ROM::Compile()
{
    std::string html_final = "";
    Category* cat = this->category;
    std::string relativepath = "roms/" + cat->foldername + "/";
    std::string projoutpath = relativepath + this->filename + ".html";
    std::string html_md = "";
    std::string desccopy = this->description;
    const char* mdstr = desccopy.c_str(); // TODO?: Potentially need to sanitize strings before sending to md_html
    std::vector<std::string> images;
    std::vector<std::string> youtubes;
    std::vector<std::string> mp4s;

    // Replace most of the basic page info
    html_final = string_fromfile("templates/rom.html");
    strreplace_all(html_final, "_TEMPLATE_ROMS_TITLE_", this->displayname);
    if (this->date != 0)
    {
        strreplace_all(html_final, "_TEMPLATE_ROMS_DATESECTION_", string_fromfile("templates/rom_date.html"));
        strreplace_all(html_final, "_TEMPLATE_ROMS_DATE_", date_tostring(this->date));
    }
    else
        strreplace_all(html_final, "_TEMPLATE_ROMS_DATESECTION_", "");
    if (this->agerating != "")
    {
        strreplace_all(html_final, "_TEMPLATE_ROMS_AGERATINGSECTION_", string_fromfile("templates/rom_age.html"));
        strreplace_all(html_final, "_TEMPLATE_ROMS_AGERATING_", this->agerating);
        if (this->contentwarning != "")
        {
            strreplace_all(html_final, "_TEMPLATE_ROMS_CONTENTWARNINGSECTION_", string_fromfile("templates/rom_contentwarn.html"));
            strreplace_all(html_final, "_TEMPLATE_ROMS_CONTENTWARNING_", this->contentwarning);
        }
        else
            strreplace_all(html_final, "_TEMPLATE_ROMS_CONTENTWARNINGSECTION_", "");
    }
    else
        strreplace_all(html_final, "_TEMPLATE_ROMS_AGERATINGSECTION_", "");
    if (this->date != 0)
    {
        strreplace_all(html_final, "_TEMPLATE_ROMS_DATESECTION_", string_fromfile("templates/rom_date.html"));
        strreplace_all(html_final, "_TEMPLATE_ROMS_DATE_", date_tostring(this->date));
    }
    else
        strreplace_all(html_final, "_TEMPLATE_ROMS_DATESECTION_", "");
    if (this->aidisclosure != "")
    {
        strreplace_all(html_final, "_TEMPLATE_ROMS_AIDISCLOSURESECTION_", string_fromfile("templates/rom_aidisclosure.html"));
        strreplace_all(html_final, "_TEMPLATE_ROMS_AIDISCLOSURE_", this->aidisclosure);
    }
    else
        strreplace_all(html_final, "_TEMPLATE_ROMS_AIDISCLOSURESECTION_", "");
    md_html(mdstr, strlen(mdstr), md4c_funcptr_handlestr, &html_md, MD_FLAG_NOHTMLBLOCKS | MD_FLAG_HEADINGAUTOID, 0, new MD_TOC_OPTIONS{});
    if (html_md.length() == 0 && this->description.length() > 0)
    {
        std::string err = "Markdown parsing was unsuccessful for '" + this->displayname + "'! Do you have some unsupported unicode?";
        printf("%s\n", err.c_str());
        exit(5);
    }
    strreplace_all(html_final, "_TEMPLATE_ROMS_DESCRIPTION_", html_md); // TODO?: Potentially need to unsanitize strings if sanitized before
    strreplace_all(html_final, "_TEMPLATE_ROMS_CATEGORY_", this->category->foldername);
    strreplace_all(html_final, "_TEMPLATE_ROM_URL_", relativepath + this->filename + ".html");
    strreplace_all(html_final, "_TEMPLATE_ROM_IMAGE_", relativepath + this->icon);
    strreplace_all(html_final, "_TEMPLATE_ROM_TOOLTIP_", this->tooltip);

    // Handle image carousel
    for (std::string str : this->images)
    {
        strreplace_all(str, "\"", "");
        if (str.find("youtube.com") != std::string::npos)
            youtubes.push_back(str.substr(str.find('=') + 1, 11));
        else if (str.find("youtu.be") != std::string::npos)
            youtubes.push_back(str.substr(str.rfind('/') + 1, 11));
        else if (str.find(".mp4") != std::string::npos)
            mp4s.push_back(str);
        else
            images.push_back(str);
    }
    if (images.size() > 0 || youtubes.size() > 0 || mp4s.size() > 0)
    {
        int i;
        std::string carousel = string_fromfile("templates/rom_carousel.html");
        std::string carousel_object = "";
        std::string carousel_list = "";
        if (images.size() > 0)
        {
            carousel_object += string_fromfile("templates/rom_carousel_object_img.html");
            strreplace_all(carousel_object, "_TEMPLATE_ROMS_CAROUSEL_IMAGE_", images[0]);
        }
        if (youtubes.size() > 0)
        {
            carousel_object += string_fromfile("templates/rom_carousel_object_youtube.html");
            strreplace_all(carousel_object, "_TEMPLATE_ROMS_CAROUSEL_YOUTUBE_", youtubes[0]);
            if (images.size() == 0)
                strreplace_all(carousel_object, "_TEMPLATE_ROMS_CAROUSEL_SHOULDDISPLAYYOUTUBE_", "");
            else
                strreplace_all(carousel_object, "_TEMPLATE_ROMS_CAROUSEL_SHOULDDISPLAYYOUTUBE_", "display:none");
        }
        if (mp4s.size() > 0)
        {
            carousel_object += string_fromfile("templates/rom_carousel_object_mp4.html");
            strreplace_all(carousel_object, "_TEMPLATE_ROMS_CAROUSEL_MP4_", mp4s[0]);
            if (images.size() == 0 && youtubes.size() == 0)
                strreplace_all(carousel_object, "_TEMPLATE_ROMS_CAROUSEL_SHOULDDISPLAYMP4_", "");
            else
                strreplace_all(carousel_object, "_TEMPLATE_ROMS_CAROUSEL_SHOULDDISPLAYMP4_", "display:none");
        }
        i=0;
        for (std::string str : mp4s)
        {
            std::string obj = string_fromfile("templates/rom_carousel_list_mp4.html");
            std::string thumb = str;
            if (i == 0 && images.size() == 0 && youtubes.size() == 0)
                strreplace_all(obj, "_TEMPLATE_ROMS_CAROUSEL_LIST_SELECTED_", "selected");
            else
                strreplace_all(obj, "_TEMPLATE_ROMS_CAROUSEL_LIST_SELECTED_", "");
            strreplace_all(thumb, ".mp4", ".jpg");
            strreplace_all(obj, "_TEMPLATE_ROMS_CAROUSEL_MP4THUMB_", thumb);
            strreplace_all(obj, "_TEMPLATE_ROMS_CAROUSEL_MP4_", str);
            carousel_list += obj;
            i++;
        }
        i=0;
        for (std::string str : youtubes)
        {
            std::string obj = string_fromfile("templates/rom_carousel_list_youtube.html");
            if (i == 0 && images.size() == 0)
                strreplace_all(obj, "_TEMPLATE_ROMS_CAROUSEL_LIST_SELECTED_", "selected");
            else
                strreplace_all(obj, "_TEMPLATE_ROMS_CAROUSEL_LIST_SELECTED_", "");
            strreplace_all(obj, "_TEMPLATE_ROMS_CAROUSEL_YOUTUBE_", str);
            carousel_list += obj;
            i++;
        }
        i=0;
        for (std::string str : images)
        {
            std::string obj = string_fromfile("templates/rom_carousel_list_img.html");
            if (i == 0)
                strreplace_all(obj, "_TEMPLATE_ROMS_CAROUSEL_LIST_SELECTED_", "selected");
            else
                strreplace_all(obj, "_TEMPLATE_ROMS_CAROUSEL_LIST_SELECTED_", "");
            strreplace_all(obj, "_TEMPLATE_ROMS_CAROUSEL_IMAGE_", str);
            carousel_list += obj;
            i++;
        }
        strreplace_all(carousel, "_TEMPLATE_ROMS_CAROUSEL_OBJECTS_", carousel_object);
        strreplace_all(carousel, "_TEMPLATE_ROMS_CAROUSEL_LIST_", carousel_list);
        strreplace_all(html_final, "_TEMPLATE_ROMS_IMAGES_", carousel);
    }
    else
        strreplace_all(html_final, "_TEMPLATE_ROMS_IMAGES_", "");

    // Handle page tags
    if (this->tags.size() > 0)
    {
        std::string pagetags = "";
        strreplace_all(html_final, "_TEMPLATE_ROMS_TAGSSECTION_", string_fromfile("templates/rom_tags.html"));
        for (std::string tag : this->tags)
        {
            std::string str = string_fromfile("templates/rom_tag.html");
            strreplace_all(str, "_TEMPLATE_TAG_URL_", "../../tags/" + sanitize_tagname(tag) + ".html");
            strreplace_all(str, "_TEMPLATE_TAG_NAME_", tag);
            pagetags += str;
            if (tag != this->tags.back())
                pagetags += ", ";
        }
        strreplace_all(html_final, "_TEMPLATE_ROMS_TAGS_", pagetags);
    }
    else
        strreplace_all(html_final, "_TEMPLATE_ROMS_TAGSSECTION_", "");

    // Handle page authors
    if (this->authors.size() > 0)
    {
        std::string pageauthors = "";
        strreplace_all(html_final, "_TEMPLATE_ROMS_AUTHORSSECTION_", string_fromfile("templates/rom_authors.html"));
        for (Page_Author* author : this->authors)
        {
            std::string str = string_fromfile("templates/rom_author.html");
            strreplace_all(str, "_TEMPLATE_AUTHOR_URL_", "../../authors/" + sanitize_tagname(author->filename) + ".html");
            strreplace_all(str, "_TEMPLATE_AUTHOR_NAME_", author->displayname);
            pageauthors += str;
            if (author != this->authors.back())
                pageauthors += ", ";
        }
        strreplace_all(html_final, "_TEMPLATE_ROMS_AUTHORS_", pageauthors);
    }
    else
        strreplace_all(html_final, "_TEMPLATE_ROMS_AUTHORSSECTION_", "");

    // Handle page genres
    if (this->genres.size() > 0)
    {
        std::string pagegenres = "";
        strreplace_all(html_final, "_TEMPLATE_ROMS_GENRESSECTION_", string_fromfile("templates/rom_genres.html"));
        for (std::string genre : this->genres)
        {
            std::string str = string_fromfile("templates/rom_genre.html");
            strreplace_all(str, "_TEMPLATE_GENRE_URL_", "../../genre/" + sanitize_tagname(genre) + ".html");
            strreplace_all(str, "_TEMPLATE_GENRE_NAME_", genre);
            pagegenres += str;
            if (genre != this->genres.back())
                pagegenres += ", ";
        }
        strreplace_all(html_final, "_TEMPLATE_ROMS_GENRES_", pagegenres);
    }
    else
        strreplace_all(html_final, "_TEMPLATE_ROMS_GENRESSECTION_", "");

    // Handle page URLS
    if (this->urls.size() > 0)
    {
        std::string url_list = "";
        for (std::string str : this->urls)
        {
            std::string obj = string_fromfile("templates/rom_url.html");
            strreplace_all(str, "\"", "");
            if (str.find("github.com") != std::string::npos)
                strreplace_all(obj, "_TEMPLATE_ROMS_URL_BADGE_", "../downloadgh.png");
            else if (str.find("gitlab.com") != std::string::npos)
                strreplace_all(obj, "_TEMPLATE_ROMS_URL_BADGE_", "../downloadgl.png");
            else if (str.find("itch.io") != std::string::npos)
                strreplace_all(obj, "_TEMPLATE_ROMS_URL_BADGE_", "../downloadit.png");
            else if (str.find("youtube.com") != std::string::npos || str.find("youtu.be") != std::string::npos)
                strreplace_all(obj, "_TEMPLATE_ROMS_URL_BADGE_", "../downloadyt.png");
            else
                strreplace_all(obj, "_TEMPLATE_ROMS_URL_BADGE_", "../downloadzp.png");
            strreplace_all(obj, "_TEMPLATE_ROMS_URL_", str);
            url_list += obj;
        }
        strreplace_all(html_final, "_TEMPLATE_ROMS_LINKS_", url_list);
    }
    else
        strreplace_all(html_final, "_TEMPLATE_ROMS_LINKS_", "");

    // Generate the page itself
    std::ofstream out(projoutpath);
    out << html_final;
    out.close();
}