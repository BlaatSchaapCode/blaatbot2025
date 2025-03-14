import{D as K,m as k,k as T,r as A,E as ht,v as s,F as V,G as mt,q as L,H as vt,I as gt,J as y,K as bt,L as xt,M as yt,S as wt,W as zt,O as Ct,o as q,T as _t,n as At,P as c,Q as r,R as l,U as v,V as D,x as F,X as St,Y as Rt,Z as Ot,$ as kt,p as Et,a0 as Lt,a1 as Ht,c as N,d as j,w as H,e as B,f as M,N as It,a2 as Pt,_ as Tt,a3 as I,z as P,B as $t,C as Ft,A as Nt}from"./create-pinia-singleton-400c8f5f.js";import{N as jt,k as Bt,g as Mt,a as Kt}from"./Scrollbar-dcf5a4d3.js";const E=K("n-notification-provider"),Vt=k({name:"NotificationContainer",props:{scrollable:{type:Boolean,required:!0},placement:{type:String,required:!0}},setup(){const{mergedThemeRef:n,mergedClsPrefixRef:e,wipTransitionCountRef:i}=T(E),o=A(null);return ht(()=>{var a,f;i.value>0?(a=o==null?void 0:o.value)===null||a===void 0||a.classList.add("transitioning"):(f=o==null?void 0:o.value)===null||f===void 0||f.classList.remove("transitioning")}),{selfRef:o,mergedTheme:n,mergedClsPrefix:e,transitioning:i}},render(){const{$slots:n,scrollable:e,mergedClsPrefix:i,mergedTheme:o,placement:a}=this;return s("div",{ref:"selfRef",class:[`${i}-notification-container`,e&&`${i}-notification-container--scrollable`,`${i}-notification-container--${a}`]},e?s(jt,{theme:o.peers.Scrollbar,themeOverrides:o.peerOverrides.Scrollbar,contentStyle:{overflow:"hidden"}},n):n)}}),qt={info:()=>s(yt,null),success:()=>s(wt,null),warning:()=>s(zt,null),error:()=>s(Ct,null),default:()=>null},$={closable:{type:Boolean,default:!0},type:{type:String,default:"default"},avatar:Function,title:[String,Function],description:[String,Function],content:[String,Function],meta:[String,Function],action:[String,Function],onClose:{type:Function,required:!0},keepAliveOnHover:Boolean,onMouseenter:Function,onMouseleave:Function},Dt=Bt($),Xt=k({name:"Notification",props:$,setup(n){const{mergedClsPrefixRef:e,mergedThemeRef:i,props:o}=T(E),{inlineThemeDisabled:a,mergedRtlRef:f}=V(),p=mt("Notification",f,e),g=L(()=>{const{type:b}=n,{self:{color:w,textColor:z,closeIconColor:C,closeIconColorHover:u,closeIconColorPressed:t,headerTextColor:d,descriptionTextColor:m,actionTextColor:x,borderRadius:S,headerFontWeight:R,boxShadow:_,lineHeight:W,fontSize:G,closeMargin:J,closeSize:Q,width:U,padding:Y,closeIconSize:Z,closeBorderRadius:tt,closeColorHover:et,closeColorPressed:nt,titleFontSize:ot,metaFontSize:it,descriptionFontSize:at,[vt("iconColor",b)]:rt},common:{cubicBezierEaseOut:st,cubicBezierEaseIn:lt,cubicBezierEaseInOut:ct}}=i.value,{left:ft,right:dt,top:ut,bottom:pt}=Mt(Y);return{"--n-color":w,"--n-font-size":G,"--n-text-color":z,"--n-description-text-color":m,"--n-action-text-color":x,"--n-title-text-color":d,"--n-title-font-weight":R,"--n-bezier":ct,"--n-bezier-ease-out":st,"--n-bezier-ease-in":lt,"--n-border-radius":S,"--n-box-shadow":_,"--n-close-border-radius":tt,"--n-close-color-hover":et,"--n-close-color-pressed":nt,"--n-close-icon-color":C,"--n-close-icon-color-hover":u,"--n-close-icon-color-pressed":t,"--n-line-height":W,"--n-icon-color":rt,"--n-close-margin":J,"--n-close-size":Q,"--n-close-icon-size":Z,"--n-width":U,"--n-padding-left":ft,"--n-padding-right":dt,"--n-padding-top":ut,"--n-padding-bottom":pt,"--n-title-font-size":ot,"--n-meta-font-size":it,"--n-description-font-size":at}}),h=a?gt("notification",L(()=>n.type[0]),g,o):void 0;return{mergedClsPrefix:e,showAvatar:L(()=>n.avatar||n.type!=="default"),handleCloseClick(){n.onClose()},rtlEnabled:p,cssVars:a?void 0:g,themeClass:h==null?void 0:h.themeClass,onRender:h==null?void 0:h.onRender}},render(){var n;const{mergedClsPrefix:e}=this;return(n=this.onRender)===null||n===void 0||n.call(this),s("div",{class:[`${e}-notification-wrapper`,this.themeClass],onMouseenter:this.onMouseenter,onMouseleave:this.onMouseleave,style:this.cssVars},s("div",{class:[`${e}-notification`,this.rtlEnabled&&`${e}-notification--rtl`,this.themeClass,{[`${e}-notification--closable`]:this.closable,[`${e}-notification--show-avatar`]:this.showAvatar}],style:this.cssVars},this.showAvatar?s("div",{class:`${e}-notification__avatar`},this.avatar?y(this.avatar):this.type!=="default"?s(bt,{clsPrefix:e},{default:()=>qt[this.type]()}):null):null,this.closable?s(xt,{clsPrefix:e,class:`${e}-notification__close`,onClick:this.handleCloseClick}):null,s("div",{ref:"bodyRef",class:`${e}-notification-main`},this.title?s("div",{class:`${e}-notification-main__header`},y(this.title)):null,this.description?s("div",{class:`${e}-notification-main__description`},y(this.description)):null,this.content?s("pre",{class:`${e}-notification-main__content`},y(this.content)):null,this.meta||this.action?s("div",{class:`${e}-notification-main-footer`},this.meta?s("div",{class:`${e}-notification-main-footer__meta`},y(this.meta)):null,this.action?s("div",{class:`${e}-notification-main-footer__action`},y(this.action)):null):null)))}}),Wt=Object.assign(Object.assign({},$),{duration:Number,onClose:Function,onLeave:Function,onAfterEnter:Function,onAfterLeave:Function,onHide:Function,onAfterShow:Function,onAfterHide:Function}),Gt=k({name:"NotificationEnvironment",props:Object.assign(Object.assign({},Wt),{internalKey:{type:String,required:!0},onInternalAfterLeave:{type:Function,required:!0}}),setup(n){const{wipTransitionCountRef:e}=T(E),i=A(!0);let o=null;function a(){i.value=!1,o&&window.clearTimeout(o)}function f(t){e.value++,At(()=>{t.style.height=`${t.offsetHeight}px`,t.style.maxHeight="0",t.style.transition="none",t.offsetHeight,t.style.transition="",t.style.maxHeight=t.style.height})}function p(t){e.value--,t.style.height="",t.style.maxHeight="";const{onAfterEnter:d,onAfterShow:m}=n;d&&d(),m&&m()}function g(t){e.value++,t.style.maxHeight=`${t.offsetHeight}px`,t.style.height=`${t.offsetHeight}px`,t.offsetHeight}function h(t){const{onHide:d}=n;d&&d(),t.style.maxHeight="0",t.offsetHeight}function b(){e.value--;const{onAfterLeave:t,onInternalAfterLeave:d,onAfterHide:m,internalKey:x}=n;t&&t(),d(x),m&&m()}function w(){const{duration:t}=n;t&&(o=window.setTimeout(a,t))}function z(t){t.currentTarget===t.target&&o!==null&&(window.clearTimeout(o),o=null)}function C(t){t.currentTarget===t.target&&w()}function u(){const{onClose:t}=n;t?Promise.resolve(t()).then(d=>{d!==!1&&a()}):a()}return q(()=>{n.duration&&(o=window.setTimeout(a,n.duration))}),{show:i,hide:a,handleClose:u,handleAfterLeave:b,handleLeave:h,handleBeforeLeave:g,handleAfterEnter:p,handleBeforeEnter:f,handleMouseenter:z,handleMouseleave:C}},render(){return s(_t,{name:"notification-transition",appear:!0,onBeforeEnter:this.handleBeforeEnter,onAfterEnter:this.handleAfterEnter,onBeforeLeave:this.handleBeforeLeave,onLeave:this.handleLeave,onAfterLeave:this.handleAfterLeave},{default:()=>this.show?s(Xt,Object.assign({},Kt(this.$props,Dt),{onClose:this.handleClose,onMouseenter:this.duration&&this.keepAliveOnHover?this.handleMouseenter:void 0,onMouseleave:this.duration&&this.keepAliveOnHover?this.handleMouseleave:void 0})):null})}}),Jt=c([r("notification-container",`
 z-index: 4000;
 position: fixed;
 overflow: visible;
 display: flex;
 flex-direction: column;
 align-items: flex-end;
 `,[c(">",[r("scrollbar",`
 width: initial;
 overflow: visible;
 height: -moz-fit-content !important;
 height: fit-content !important;
 max-height: 100vh !important;
 `,[c(">",[r("scrollbar-container",`
 height: -moz-fit-content !important;
 height: fit-content !important;
 max-height: 100vh !important;
 `,[r("scrollbar-content",`
 padding-top: 12px;
 padding-bottom: 33px;
 `)])])])]),l("top, top-right, top-left",`
 top: 12px;
 `,[c("&.transitioning >",[r("scrollbar",[c(">",[r("scrollbar-container",`
 min-height: 100vh !important;
 `)])])])]),l("bottom, bottom-right, bottom-left",`
 bottom: 12px;
 `,[c(">",[r("scrollbar",[c(">",[r("scrollbar-container",[r("scrollbar-content",`
 padding-bottom: 12px;
 `)])])])]),r("notification-wrapper",`
 display: flex;
 align-items: flex-end;
 margin-bottom: 0;
 margin-top: 12px;
 `)]),l("top, bottom",`
 left: 50%;
 transform: translateX(-50%);
 `,[r("notification-wrapper",[c("&.notification-transition-enter-from, &.notification-transition-leave-to",`
 transform: scale(0.85);
 `),c("&.notification-transition-leave-from, &.notification-transition-enter-to",`
 transform: scale(1);
 `)])]),l("top",[r("notification-wrapper",`
 transform-origin: top center;
 `)]),l("bottom",[r("notification-wrapper",`
 transform-origin: bottom center;
 `)]),l("top-right, bottom-right",[r("notification",`
 margin-left: 28px;
 margin-right: 16px;
 `)]),l("top-left, bottom-left",[r("notification",`
 margin-left: 16px;
 margin-right: 28px;
 `)]),l("top-right",`
 right: 0;
 `,[O("top-right")]),l("top-left",`
 left: 0;
 `,[O("top-left")]),l("bottom-right",`
 right: 0;
 `,[O("bottom-right")]),l("bottom-left",`
 left: 0;
 `,[O("bottom-left")]),l("scrollable",[l("top-right",`
 top: 0;
 `),l("top-left",`
 top: 0;
 `),l("bottom-right",`
 bottom: 0;
 `),l("bottom-left",`
 bottom: 0;
 `)]),r("notification-wrapper",`
 margin-bottom: 12px;
 `,[c("&.notification-transition-enter-from, &.notification-transition-leave-to",`
 opacity: 0;
 margin-top: 0 !important;
 margin-bottom: 0 !important;
 `),c("&.notification-transition-leave-from, &.notification-transition-enter-to",`
 opacity: 1;
 `),c("&.notification-transition-leave-active",`
 transition:
 background-color .3s var(--n-bezier),
 color .3s var(--n-bezier),
 opacity .3s var(--n-bezier),
 transform .3s var(--n-bezier-ease-in),
 max-height .3s var(--n-bezier),
 margin-top .3s linear,
 margin-bottom .3s linear,
 box-shadow .3s var(--n-bezier);
 `),c("&.notification-transition-enter-active",`
 transition:
 background-color .3s var(--n-bezier),
 color .3s var(--n-bezier),
 opacity .3s var(--n-bezier),
 transform .3s var(--n-bezier-ease-out),
 max-height .3s var(--n-bezier),
 margin-top .3s linear,
 margin-bottom .3s linear,
 box-shadow .3s var(--n-bezier);
 `)]),r("notification",`
 background-color: var(--n-color);
 color: var(--n-text-color);
 transition:
 background-color .3s var(--n-bezier),
 color .3s var(--n-bezier),
 opacity .3s var(--n-bezier),
 box-shadow .3s var(--n-bezier);
 font-family: inherit;
 font-size: var(--n-font-size);
 font-weight: 400;
 position: relative;
 display: flex;
 overflow: hidden;
 flex-shrink: 0;
 padding-left: var(--n-padding-left);
 padding-right: var(--n-padding-right);
 width: var(--n-width);
 max-width: calc(100vw - 16px - 16px);
 border-radius: var(--n-border-radius);
 box-shadow: var(--n-box-shadow);
 box-sizing: border-box;
 opacity: 1;
 `,[v("avatar",[r("icon",`
 color: var(--n-icon-color);
 `),r("base-icon",`
 color: var(--n-icon-color);
 `)]),l("show-avatar",[r("notification-main",`
 margin-left: 40px;
 width: calc(100% - 40px); 
 `)]),l("closable",[r("notification-main",[c("> *:first-child",`
 padding-right: 20px;
 `)]),v("close",`
 position: absolute;
 top: 0;
 right: 0;
 margin: var(--n-close-margin);
 transition:
 background-color .3s var(--n-bezier),
 color .3s var(--n-bezier);
 `)]),v("avatar",`
 position: absolute;
 top: var(--n-padding-top);
 left: var(--n-padding-left);
 width: 28px;
 height: 28px;
 font-size: 28px;
 display: flex;
 align-items: center;
 justify-content: center;
 `,[r("icon","transition: color .3s var(--n-bezier);")]),r("notification-main",`
 padding-top: var(--n-padding-top);
 padding-bottom: var(--n-padding-bottom);
 box-sizing: border-box;
 display: flex;
 flex-direction: column;
 margin-left: 8px;
 width: calc(100% - 8px);
 `,[r("notification-main-footer",`
 display: flex;
 align-items: center;
 justify-content: space-between;
 margin-top: 12px;
 `,[v("meta",`
 font-size: var(--n-meta-font-size);
 transition: color .3s var(--n-bezier-ease-out);
 color: var(--n-description-text-color);
 `),v("action",`
 cursor: pointer;
 transition: color .3s var(--n-bezier-ease-out);
 color: var(--n-action-text-color);
 `)]),v("header",`
 font-weight: var(--n-title-font-weight);
 font-size: var(--n-title-font-size);
 transition: color .3s var(--n-bezier-ease-out);
 color: var(--n-title-text-color);
 `),v("description",`
 margin-top: 8px;
 font-size: var(--n-description-font-size);
 white-space: pre-wrap;
 word-wrap: break-word;
 transition: color .3s var(--n-bezier-ease-out);
 color: var(--n-description-text-color);
 `),v("content",`
 line-height: var(--n-line-height);
 margin: 12px 0 0 0;
 font-family: inherit;
 white-space: pre-wrap;
 word-wrap: break-word;
 transition: color .3s var(--n-bezier-ease-out);
 color: var(--n-text-color);
 `,[c("&:first-child","margin: 0;")])])])])]);function O(n){const i=n.split("-")[1]==="left"?"calc(-100%)":"calc(100%)",o="0";return r("notification-wrapper",[c("&.notification-transition-enter-from, &.notification-transition-leave-to",`
 transform: translate(${i}, 0);
 `),c("&.notification-transition-leave-from, &.notification-transition-enter-to",`
 transform: translate(${o}, 0);
 `)])}const Qt=K("n-notification-api"),Ut=Object.assign(Object.assign({},D.props),{containerClass:String,containerStyle:[String,Object],to:[String,Object],scrollable:{type:Boolean,default:!0},max:Number,placement:{type:String,default:"top-right"},keepAliveOnHover:Boolean}),Yt=k({name:"NotificationProvider",props:Ut,setup(n){const{mergedClsPrefixRef:e}=V(n),i=A([]),o={},a=new Set;function f(u){const t=kt(),d=()=>{a.add(t),o[t]&&o[t].hide()},m=Et(Object.assign(Object.assign({},u),{key:t,destroy:d,hide:d,deactivate:d})),{max:x}=n;if(x&&i.value.length-a.size>=x){let S=!1,R=0;for(const _ of i.value){if(!a.has(_.key)){o[_.key]&&(_.destroy(),S=!0);break}R++}S||i.value.splice(R,1)}return i.value.push(m),m}const p=["info","success","warning","error"].map(u=>t=>f(Object.assign(Object.assign({},t),{type:u})));function g(u){a.delete(u),i.value.splice(i.value.findIndex(t=>t.key===u),1)}const h=D("Notification","-notification",Jt,Lt,n,e),b={create:f,info:p[0],success:p[1],warning:p[2],error:p[3],open:z,destroyAll:C},w=A(0);F(Qt,b),F(E,{props:n,mergedClsPrefixRef:e,mergedThemeRef:h,wipTransitionCountRef:w});function z(u){return f(u)}function C(){Object.values(i.value).forEach(u=>{u.hide()})}return Object.assign({mergedClsPrefix:e,notificationList:i,notificationRefs:o,handleAfterLeave:g},b)},render(){var n,e,i;const{placement:o}=this;return s(Ot,null,(e=(n=this.$slots).default)===null||e===void 0?void 0:e.call(n),this.notificationList.length?s(St,{to:(i=this.to)!==null&&i!==void 0?i:"body"},s(Vt,{class:this.containerClass,style:this.containerStyle,scrollable:this.scrollable&&o!=="top"&&o!=="bottom",placement:o},{default:()=>this.notificationList.map(a=>s(Gt,Object.assign({ref:f=>{const p=a.key;f===null?delete this.notificationRefs[p]:this.notificationRefs[p]=f}},Rt(a,["destroy","hide","deactivate"]),{internalKey:a.key,onInternalAfterLeave:this.handleAfterLeave,keepAliveOnHover:a.keepAliveOnHover===void 0?this.keepAliveOnHover:a.keepAliveOnHover})))})):null)}}),Zt={__name:"Embedded",props:{componentName:{type:String,default:null},componentId:{type:String,default:null}},setup(n){const e={ChatLog:I(()=>P(()=>import("./ChatLog-f3ea51b0.js"),["assets/ChatLog-f3ea51b0.js","assets/datetime-78deadb9.js","assets/_setToArray-585f4e66.js","assets/create-pinia-singleton-400c8f5f.js","assets/_baseFindIndex-15a8234c.js","assets/ChatLog-15828f3c.css"])),Polls:I(()=>P(()=>import("./Polls-b0748097.js"),["assets/Polls-b0748097.js","assets/datetime-78deadb9.js","assets/create-pinia-singleton-400c8f5f.js","assets/Dropdown-cf1f2c7a.js","assets/_setToArray-585f4e66.js","assets/Scrollbar-dcf5a4d3.js"])),Status:I(()=>P(()=>import("./Status-3f2625bf.js"),["assets/Status-3f2625bf.js","assets/create-pinia-singleton-400c8f5f.js","assets/Scrollbar-dcf5a4d3.js"]))},i=n,o=A(null);return q(()=>{o.value=Ht(e[i.componentName]||null)}),(a,f)=>(N(),j(Tt,null,{default:H(()=>[B(M(Yt),null,{default:H(()=>[B(M(It),null,{default:H(()=>[(N(),j(Pt(o.value),{"component-id":i.componentId},null,8,["component-id"]))]),_:1})]),_:1})]),_:1}))}},X=$t();X.use(Ft);const te=document.querySelectorAll("div.vue-embed");for(const n of te){const e=Nt(Zt,{componentName:n.dataset.component,componentId:n.dataset.componentId});e.use(X),e.mount(n)}export{Qt as n};
