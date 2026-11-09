var STATE_IDLE = 0;
var STATE_FADINGOUT = 1;
var STATE_FADINGIN = 2;

var global_carouselstate = STATE_IDLE;
var global_carouseltarget = null;

function main()
{
    var i;

    // Category button collapser
    var colbuts = document.getElementsByClassName("collapse-button");
    for (i = 0; i < colbuts.length; i++)
    {
        colbuts[i].parentElement.nextElementSibling.style.maxHeight = colbuts[i].parentElement.nextElementSibling.scrollHeight + "px";
        colbuts[i].addEventListener("click", 
            function()
            {
                this.classList.toggle("active");
                var content = this.parentElement.nextElementSibling;
                if (!(content.style.maxHeight === "0px"))
                {
                    setTimeout(HideElementsInCollapsedCategory, 200, content);
                    content.style.maxHeight = "0px";
                    this.children[0].src = "roms/collapse-closed.png";
                }
                else
                {
                    content.style.visibility = 'visible';
                    content.style.maxHeight = content.scrollHeight + "px";
                    this.children[0].src = "roms/collapse-open.png";
                }
            }
        );
    }

    // Handle category section resize
    window.addEventListener('resize', 
        function(event) {
            for (i = 0; i < colbuts.length; i++)
                colbuts[i].parentElement.nextElementSibling.style.maxHeight = colbuts[i].parentElement.nextElementSibling.scrollHeight + "px";
        }, true
    );

    // Handle image carousel
    var carousellist = document.getElementsByClassName("carousel-list")[0];
    var carouselobject = document.getElementsByClassName("carousel-object")[0];
    if (carouselobject != null)
    {
        var carouselimage = carouselobject.children[0].children[0];
        var carouselvideo = carouselobject.children[1];
        var projobjbuts = document.getElementsByClassName("carousel-list-object");
        for (i = 0; i < projobjbuts.length; i++)
        {
            projobjbuts[i].addEventListener("click",
                function()
                {
                    var offsets;
                    var oldselected = document.getElementsByClassName("carousel-list-object selected")[0];
                    if (oldselected == this || global_carouselstate == STATE_FADINGIN)
                        return;
                    oldselected.classList.remove("selected");
                    global_carouseltarget = this;
                    this.classList.add("selected")
                    if (global_carouselstate == STATE_IDLE)
                    {
                        FadeObject(carouselimage);
                        if (this.classList.contains("video") && oldselected.classList.contains("video"))
                            SetIFrameVideo(carouselvideo, "https://www.youtube.com/embed/" + global_carouseltarget.id);
                        else if (this.classList.contains("videomp4") && oldselected.classList.contains("videomp4"))
                        FadeObject(carouselvideo);
                        setTimeout(ChangeObject, 250, carouselimage, carouselvideo, this);
                        global_carouselstate = STATE_FADINGOUT;
                    }
                    if (carousellist.getBoundingClientRect().right < this.getBoundingClientRect().right)
                        carousellist.scrollBy({
                            top: 0,
                            left: this.getBoundingClientRect().right - carousellist.getBoundingClientRect().right,
                            behavior: "smooth",
                        });
                    else if (this.getBoundingClientRect().left < carousellist.getBoundingClientRect().left)
                        carousellist.scrollBy({
                            top: 0,
                            left: (this.getBoundingClientRect().left - carousellist.getBoundingClientRect().left) - 3,
                            behavior: "smooth",
                        });
                }
            );
        }
    }
}

function HideElementsInCollapsedCategory(content)
{
    if (content.style.maxHeight === "0px")
        content.style.visibility = 'hidden';
}

function SetObjectVisiblity(content, visible)
{
    if (content == null)
        return;
    if (visible)
        content.style.display = 'block';
    else
        content.style.display = 'none';
}

function FadeObject(content, fadein)
{
    if (content == null)
        return;
    if (fadein)
    {
        content.classList.remove("fadeout");
        content.classList.remove("fadein");
        content.offsetWidth;
        content.classList.add("fadein");
    }
    else
    {
        content.classList.remove("fadein");
        content.classList.remove("fadeout");
        content.offsetWidth;
        content.classList.add("fadeout");
    }
}

function ChangeObject(image, video, src)
{
    global_carouselstate = STATE_FADINGIN;
    if (src.classList.contains("img"))
    {
        SetIFrameVideo(video, "");
        image.src = global_carouseltarget.id;
        console.log(image.parentNode)
        image.parentNode.href = image.src;
        SetObjectVisiblity(image, false);
        SetObjectVisiblity(video, false);
        FadeObject(image, true);
        SetObjectVisiblity(image, true);
        setTimeout(FinishFadein, 200);
    }
    else
    {
        SetIFrameVideo(video, "");
        SetObjectVisiblity(image, false);
        SetObjectVisiblity(video, false);
        FadeObject(video, true);
        SetObjectVisiblity(video, true);
        setTimeout(FinishFadein, 200, video, "https://www.youtube.com/embed/" + global_carouseltarget.id);
    }
}

function FinishFadein(video, src)
{
    global_carouselstate = STATE_IDLE;
    global_carouseltarget = null;
    if (video)
        SetIFrameVideo(video, src);
}

function SetIFrameVideo(video, src)
{
    if (video == null)
        return;
    var container = video.parentNode;
    console.log(src);
    video.remove();
    if (src.endsWith(".mp4"))
        src = src.replace("https://www.youtube.com/embed/", "");
    video.setAttribute("src", src);
    container.append(video);
}

main();